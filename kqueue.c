/*
 * kqueue: 1:1 bindings of <sys/event.h> — kqueue(), kevent(), kevent64(),
 * EV_SET(), EV_SET64() — plus kqueue_errno() to carry errno across the
 * PHP/C boundary.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_network.h"
#include "ext/standard/info.h"
#include "php_kqueue.h"

#if __has_include("ext/sockets/php_sockets.h")
# include "ext/sockets/php_sockets.h"
# define KQ_HAVE_SOCKETS 1
#endif

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

#if SIZEOF_ZEND_LONG != 8
# error "kqueue requires a 64-bit zend_long: kevent64_s carries 64-bit fields"
#endif

/* NOTE_REAP and NOTE_EXIT_REPARENTED are deprecated enum values in the SDK;
 * they are still part of the header and are registered like every other constant. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "kqueue_arginfo.h"
#pragma GCC diagnostic pop

ZEND_DECLARE_MODULE_GLOBALS(kqueue)

static zend_class_entry *kq_kevent_ce;
static zend_class_entry *kq_kevent64_s_ce;
static zend_class_entry *kq_timespec_ce;

/* Declared property slots, in stub order. */
enum { KQ_IDENT, KQ_FILTER, KQ_FLAGS, KQ_FFLAGS, KQ_DATA, KQ_UDATA, KQ_EXT };
enum { KQ_TV_SEC, KQ_TV_NSEC };

static const char *const kq_kevent_props[] = { "ident", "filter", "flags", "fflags", "data", "udata" };

static bool kq_read_long(zend_object *obj, uint32_t slot, const char *name, zend_long *out)
{
	zval *zv = OBJ_PROP_NUM(obj, slot);

	ZVAL_DEREF(zv);
	if (UNEXPECTED(Z_TYPE_P(zv) != IS_LONG)) {
		zend_throw_error(NULL, "Typed property %s::$%s must not be accessed before initialization",
			ZSTR_VAL(obj->ce->name), name);
		return false;
	}
	*out = Z_LVAL_P(zv);
	return true;
}

/* A narrow field accepts its value in either the signed or the unsigned reading
 * of its C width, as C's implicit conversion does for in-range values; anything
 * wider would lose bits and is rejected. arg_num > 0 reports against a function
 * argument, otherwise against obj's property. */
static bool kq_range(zend_long v, zend_long lo, zend_long hi, uint32_t arg_num, const zend_object *obj, const char *name)
{
	if (EXPECTED(v >= lo && v <= hi)) {
		return true;
	}
	if (arg_num) {
		zend_argument_value_error(arg_num, "must be between " ZEND_LONG_FMT " and " ZEND_LONG_FMT, lo, hi);
	} else {
		zend_value_error("%s::$%s must be between " ZEND_LONG_FMT " and " ZEND_LONG_FMT,
			ZSTR_VAL(obj->ce->name), name, lo, hi);
	}
	return false;
}

/* filter is int16_t, flags uint16_t, fflags uint32_t in both kevent and kevent64_s. */
static bool kq_check_narrow(zend_long filter, zend_long flags, zend_long fflags, uint32_t filter_arg, const zend_object *obj)
{
	return kq_range(filter, INT16_MIN, UINT16_MAX, filter_arg, obj, "filter")
		&& kq_range(flags, INT16_MIN, UINT16_MAX, filter_arg ? filter_arg + 1 : 0, obj, "flags")
		&& kq_range(fflags, INT32_MIN, UINT32_MAX, filter_arg ? filter_arg + 2 : 0, obj, "fflags");
}

static bool kq_read_fields(zend_object *obj, zend_long f[6])
{
	for (uint32_t i = 0; i < 6; i++) {
		if (!kq_read_long(obj, i, kq_kevent_props[i], &f[i])) {
			return false;
		}
	}
	return kq_check_narrow(f[KQ_FILTER], f[KQ_FLAGS], f[KQ_FFLAGS], 0, obj);
}

static bool kq_kevent_from_object(zend_object *obj, struct kevent *kev)
{
	zend_long f[6];

	if (!kq_read_fields(obj, f)) {
		return false;
	}
	EV_SET(kev, (uintptr_t) f[KQ_IDENT], (int16_t) f[KQ_FILTER], (uint16_t) f[KQ_FLAGS],
		(uint32_t) f[KQ_FFLAGS], (intptr_t) f[KQ_DATA], (void *) (intptr_t) f[KQ_UDATA]);
	return true;
}

static bool kq_kevent64_from_object(zend_object *obj, struct kevent64_s *kev)
{
	zend_long f[6];
	uint64_t ext[2];
	zval *zv;
	HashTable *ht;

	if (!kq_read_fields(obj, f)) {
		return false;
	}

	zv = OBJ_PROP_NUM(obj, KQ_EXT);
	ZVAL_DEREF(zv);
	if (UNEXPECTED(Z_TYPE_P(zv) != IS_ARRAY)) {
		zend_throw_error(NULL, "Typed property kevent64_s::$ext must not be accessed before initialization");
		return false;
	}
	ht = Z_ARRVAL_P(zv);
	if (zend_hash_num_elements(ht) != 2) {
		zend_value_error("kevent64_s::$ext must hold exactly 2 elements, %u held", zend_hash_num_elements(ht));
		return false;
	}
	for (zend_ulong i = 0; i < 2; i++) {
		zval *e = zend_hash_index_find(ht, i);

		if (!e) {
			zend_value_error("kevent64_s::$ext must be a list with keys 0 and 1");
			return false;
		}
		ZVAL_DEREF(e);
		if (Z_TYPE_P(e) != IS_LONG) {
			zend_type_error("kevent64_s::$ext[" ZEND_ULONG_FMT "] must be of type int, %s given", i, zend_zval_value_name(e));
			return false;
		}
		ext[i] = (uint64_t) Z_LVAL_P(e);
	}

	EV_SET64(kev, (uint64_t) f[KQ_IDENT], (int16_t) f[KQ_FILTER], (uint16_t) f[KQ_FLAGS],
		(uint32_t) f[KQ_FFLAGS], (int64_t) f[KQ_DATA], (uint64_t) f[KQ_UDATA], ext[0], ext[1]);
	return true;
}

/* Slot writes go through ZEND_TRY_ASSIGN_* so references held on a property
 * (including typed references) are assigned through, as a normal write would. */
static void kq_store_long(zend_object *obj, uint32_t slot, zend_long v)
{
	ZEND_TRY_ASSIGN_LONG(OBJ_PROP_NUM(obj, slot), v);
}

static void kq_kevent_to_object(const struct kevent *kev, zend_object *obj)
{
	kq_store_long(obj, KQ_IDENT, (zend_long) kev->ident);
	kq_store_long(obj, KQ_FILTER, (zend_long) kev->filter);
	kq_store_long(obj, KQ_FLAGS, (zend_long) kev->flags);
	kq_store_long(obj, KQ_FFLAGS, (zend_long) kev->fflags);
	kq_store_long(obj, KQ_DATA, (zend_long) kev->data);
	kq_store_long(obj, KQ_UDATA, (zend_long) (intptr_t) kev->udata);
}

static void kq_kevent64_to_object(const struct kevent64_s *kev, zend_object *obj)
{
	zval ext;

	kq_store_long(obj, KQ_IDENT, (zend_long) kev->ident);
	kq_store_long(obj, KQ_FILTER, (zend_long) kev->filter);
	kq_store_long(obj, KQ_FLAGS, (zend_long) kev->flags);
	kq_store_long(obj, KQ_FFLAGS, (zend_long) kev->fflags);
	kq_store_long(obj, KQ_DATA, (zend_long) kev->data);
	kq_store_long(obj, KQ_UDATA, (zend_long) kev->udata);

	array_init_size(&ext, 2);
	add_next_index_long(&ext, (zend_long) kev->ext[0]);
	add_next_index_long(&ext, (zend_long) kev->ext[1]);
	ZEND_TRY_ASSIGN_ARR(OBJ_PROP_NUM(obj, KQ_EXT), Z_ARR(ext));
}

/* The stub cannot declare a non-empty array default, so ext starts as [0, 0] here. */
static zend_object *kq_kevent64_s_create(zend_class_entry *ce)
{
	zend_object *obj = zend_objects_new(ce);
	zval *ext;

	object_properties_init(obj, ce);
	ext = OBJ_PROP_NUM(obj, KQ_EXT);
	array_init_size(ext, 2);
	add_next_index_long(ext, 0);
	add_next_index_long(ext, 0);
	return obj;
}

static bool kq_timespec_from_object(zend_object *obj, struct timespec *ts)
{
	zend_long sec, nsec;

	if (!kq_read_long(obj, KQ_TV_SEC, "tv_sec", &sec) || !kq_read_long(obj, KQ_TV_NSEC, "tv_nsec", &nsec)) {
		return false;
	}
	ts->tv_sec = (time_t) sec;
	ts->tv_nsec = (long) nsec;
	return true;
}

/* Arguments shared by kevent() and kevent64(): kq #1, changelist #2, nchanges #3, eventlist #4, nevents #5. */
static bool kq_check_call(zend_long kq, HashTable *changelist, zend_long nchanges, zval *eventlist, zend_long nevents)
{
	zend_long held = (zend_long) zend_hash_num_elements(changelist);
	zval *in = eventlist;

	if (kq < INT_MIN || kq > INT_MAX) {
		zend_argument_value_error(1, "must be between %d and %d", INT_MIN, INT_MAX);
		return false;
	}
	if (nchanges < 0 || nchanges > held || nchanges > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and the element count of argument #2 ($changelist) (" ZEND_LONG_FMT ")", held);
		return false;
	}
	ZVAL_DEREF(in);
	if (Z_TYPE_P(in) != IS_NULL && Z_TYPE_P(in) != IS_ARRAY) {
		zend_argument_type_error(4, "must be of type ?array, %s given", zend_zval_value_name(in));
		return false;
	}
	if (nevents < 0 || nevents > INT_MAX) {
		zend_argument_value_error(5, "must be between 0 and %d", INT_MAX);
		return false;
	}
	return true;
}

ZEND_FUNCTION(kqueue)
{
	int fd;

	ZEND_PARSE_PARAMETERS_NONE();

	fd = kqueue();
	if (fd == -1) {
		KQUEUE_G(last_errno) = errno;
	}
	RETURN_LONG(fd);
}

ZEND_FUNCTION(kevent)
{
	zend_long kq, nchanges, nevents, i = 0;
	HashTable *changelist;
	zval *eventlist, *entry;
	zend_object *timeout = NULL;
	struct kevent *changes = NULL, *events = NULL;
	struct timespec ts;
	int n;

	ZEND_PARSE_PARAMETERS_START(6, 6)
		Z_PARAM_LONG(kq)
		Z_PARAM_ARRAY_HT(changelist)
		Z_PARAM_LONG(nchanges)
		Z_PARAM_ZVAL(eventlist)
		Z_PARAM_LONG(nevents)
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(timeout, kq_timespec_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (!kq_check_call(kq, changelist, nchanges, eventlist, nevents)
		|| (timeout && !kq_timespec_from_object(timeout, &ts))) {
		RETURN_THROWS();
	}

	if (nchanges) {
		changes = safe_emalloc(nchanges, sizeof(struct kevent), 0);
		ZEND_HASH_FOREACH_VAL(changelist, entry) {
			if (i == nchanges) {
				break;
			}
			ZVAL_DEREF(entry);
			if (Z_TYPE_P(entry) != IS_OBJECT || Z_OBJCE_P(entry) != kq_kevent_ce) {
				zend_argument_type_error(2, "must contain only kevent, %s given at position " ZEND_LONG_FMT,
					zend_zval_value_name(entry), i);
				efree(changes);
				RETURN_THROWS();
			}
			if (!kq_kevent_from_object(Z_OBJ_P(entry), &changes[i])) {
				efree(changes);
				RETURN_THROWS();
			}
			i++;
		} ZEND_HASH_FOREACH_END();
	}
	if (nevents) {
		events = safe_emalloc(nevents, sizeof(struct kevent), 0);
	}

	n = kevent((int) kq, changes, (int) nchanges, events, (int) nevents, timeout ? &ts : NULL);

	if (n == -1) {
		KQUEUE_G(last_errno) = errno;
	} else {
		zval list, ev;

		array_init_size(&list, (uint32_t) n);
		for (int j = 0; j < n; j++) {
			object_init_ex(&ev, kq_kevent_ce);
			kq_kevent_to_object(&events[j], Z_OBJ(ev));
			add_next_index_zval(&list, &ev);
		}
		ZEND_TRY_ASSIGN_REF_ARR(eventlist, Z_ARR(list));
	}

	if (changes) {
		efree(changes);
	}
	if (events) {
		efree(events);
	}
	RETURN_LONG(n);
}

ZEND_FUNCTION(kevent64)
{
	zend_long kq, nchanges, nevents, flags, i = 0;
	HashTable *changelist;
	zval *eventlist, *entry;
	zend_object *timeout = NULL;
	struct kevent64_s *changes = NULL, *events = NULL;
	struct timespec ts;
	int n;

	ZEND_PARSE_PARAMETERS_START(7, 7)
		Z_PARAM_LONG(kq)
		Z_PARAM_ARRAY_HT(changelist)
		Z_PARAM_LONG(nchanges)
		Z_PARAM_ZVAL(eventlist)
		Z_PARAM_LONG(nevents)
		Z_PARAM_LONG(flags)
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(timeout, kq_timespec_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (!kq_check_call(kq, changelist, nchanges, eventlist, nevents)
		|| !kq_range(flags, INT32_MIN, UINT32_MAX, 6, NULL, NULL)
		|| (timeout && !kq_timespec_from_object(timeout, &ts))) {
		RETURN_THROWS();
	}

	if (nchanges) {
		changes = safe_emalloc(nchanges, sizeof(struct kevent64_s), 0);
		ZEND_HASH_FOREACH_VAL(changelist, entry) {
			if (i == nchanges) {
				break;
			}
			ZVAL_DEREF(entry);
			if (Z_TYPE_P(entry) != IS_OBJECT || Z_OBJCE_P(entry) != kq_kevent64_s_ce) {
				zend_argument_type_error(2, "must contain only kevent64_s, %s given at position " ZEND_LONG_FMT,
					zend_zval_value_name(entry), i);
				efree(changes);
				RETURN_THROWS();
			}
			if (!kq_kevent64_from_object(Z_OBJ_P(entry), &changes[i])) {
				efree(changes);
				RETURN_THROWS();
			}
			i++;
		} ZEND_HASH_FOREACH_END();
	}
	if (nevents) {
		events = safe_emalloc(nevents, sizeof(struct kevent64_s), 0);
	}

	n = kevent64((int) kq, changes, (int) nchanges, events, (int) nevents, (unsigned int) flags, timeout ? &ts : NULL);

	if (n == -1) {
		KQUEUE_G(last_errno) = errno;
	} else {
		zval list, ev;

		array_init_size(&list, (uint32_t) n);
		for (int j = 0; j < n; j++) {
			object_init_ex(&ev, kq_kevent64_s_ce);
			kq_kevent64_to_object(&events[j], Z_OBJ(ev));
			add_next_index_zval(&list, &ev);
		}
		ZEND_TRY_ASSIGN_REF_ARR(eventlist, Z_ARR(list));
	}

	if (changes) {
		efree(changes);
	}
	if (events) {
		efree(events);
	}
	RETURN_LONG(n);
}

/* ident for EV_SET/EV_SET64: an int as given, or the fd behind a stream or Socket. */
static bool kq_zval_to_ident(zval *zident, uint32_t arg_num, zend_long *ident)
{
	switch (Z_TYPE_P(zident)) {
		case IS_LONG:
			*ident = Z_LVAL_P(zident);
			return true;

		case IS_RESOURCE: {
			php_stream *stream = (php_stream *) zend_fetch_resource2_ex(
				zident, NULL, php_file_le_stream(), php_file_le_pstream()
			);
			php_socket_t stream_fd = -1;

			if (stream == NULL) {
				zend_argument_type_error(arg_num, "must be a valid stream resource");
				return false;
			}

			if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
					(void **) &stream_fd, 0) != SUCCESS || stream_fd < 0) {
				zend_argument_value_error(arg_num, "must be a stream backed by a file descriptor");
				return false;
			}

			*ident = (zend_long) stream_fd;
			return true;
		}

#ifdef KQ_HAVE_SOCKETS
		case IS_OBJECT: {
			/* Resolved by name so kqueue.so never links against ext/sockets symbols. */
			zend_class_entry *socket_class = zend_hash_str_find_ptr(CG(class_table), "socket", sizeof("socket") - 1);

			if (socket_class != NULL && Z_OBJCE_P(zident) == socket_class) {
				php_socket *socket = Z_SOCKET_P(zident);

				if (socket->bsd_socket < 0) {
					zend_argument_value_error(arg_num, "has already been closed");
					return false;
				}

				*ident = (zend_long) socket->bsd_socket;
				return true;
			}
			break;
		}
#endif
	}

	zend_argument_type_error(arg_num, "must be of type Socket|resource|int, %s given", zend_zval_value_name(zident));
	return false;
}

ZEND_FUNCTION(EV_SET)
{
	zend_object *kevp;
	zval *zident;
	zend_long ident, filter, flags, fflags, data, udata;
	struct kevent kev;

	ZEND_PARSE_PARAMETERS_START(7, 7)
		Z_PARAM_OBJ_OF_CLASS(kevp, kq_kevent_ce)
		Z_PARAM_ZVAL(zident)
		Z_PARAM_LONG(filter)
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(fflags)
		Z_PARAM_LONG(data)
		Z_PARAM_LONG(udata)
	ZEND_PARSE_PARAMETERS_END();

	if (!kq_zval_to_ident(zident, 2, &ident) || !kq_check_narrow(filter, flags, fflags, 3, NULL)) {
		RETURN_THROWS();
	}

	EV_SET(&kev, (uintptr_t) ident, (int16_t) filter, (uint16_t) flags, (uint32_t) fflags,
		(intptr_t) data, (void *) (intptr_t) udata);
	kq_kevent_to_object(&kev, kevp);
}

ZEND_FUNCTION(EV_SET64)
{
	zend_object *kevp;
	zval *zident;
	zend_long ident, filter, flags, fflags, data, udata, ext0, ext1;
	struct kevent64_s kev;

	ZEND_PARSE_PARAMETERS_START(9, 9)
		Z_PARAM_OBJ_OF_CLASS(kevp, kq_kevent64_s_ce)
		Z_PARAM_ZVAL(zident)
		Z_PARAM_LONG(filter)
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(fflags)
		Z_PARAM_LONG(data)
		Z_PARAM_LONG(udata)
		Z_PARAM_LONG(ext0)
		Z_PARAM_LONG(ext1)
	ZEND_PARSE_PARAMETERS_END();

	if (!kq_zval_to_ident(zident, 2, &ident) || !kq_check_narrow(filter, flags, fflags, 3, NULL)) {
		RETURN_THROWS();
	}

	EV_SET64(&kev, (uint64_t) ident, (int16_t) filter, (uint16_t) flags, (uint32_t) fflags,
		(int64_t) data, (uint64_t) udata, (uint64_t) ext0, (uint64_t) ext1);
	kq_kevent64_to_object(&kev, kevp);
}

ZEND_FUNCTION(kqueue_errno)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(KQUEUE_G(last_errno));
}

static PHP_GINIT_FUNCTION(kqueue)
{
#if defined(COMPILE_DL_KQUEUE) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	kqueue_globals->last_errno = 0;
}

PHP_MINIT_FUNCTION(kqueue)
{
	register_kqueue_symbols(module_number);

	kq_kevent_ce = register_class_kevent();
	kq_kevent64_s_ce = register_class_kevent64_s();
	kq_kevent64_s_ce->create_object = kq_kevent64_s_create;
	kq_timespec_ce = register_class_timespec();

	return SUCCESS;
}

PHP_RINIT_FUNCTION(kqueue)
{
#if defined(COMPILE_DL_KQUEUE) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	KQUEUE_G(last_errno) = 0;

	return SUCCESS;
}

PHP_MINFO_FUNCTION(kqueue)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "kqueue support", "enabled");
	php_info_print_table_row(2, "Version", PHP_KQUEUE_VERSION);
	php_info_print_table_end();
}

zend_module_entry kqueue_module_entry = {
	STANDARD_MODULE_HEADER,
	"kqueue",
	ext_functions,
	PHP_MINIT(kqueue),
	NULL,
	PHP_RINIT(kqueue),
	NULL,
	PHP_MINFO(kqueue),
	PHP_KQUEUE_VERSION,
	PHP_MODULE_GLOBALS(kqueue),
	PHP_GINIT(kqueue),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_KQUEUE
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(kqueue)
#endif
