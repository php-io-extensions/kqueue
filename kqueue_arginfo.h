/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: acd2cc0e601e4a480b7aff7239c97dd6ed108f94 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_kqueue, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_kevent, 0, 6, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, kq, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, changelist, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, nchanges, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, eventlist, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, nevents, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, timeout, timespec, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_kevent64, 0, 7, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, kq, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, changelist, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, nchanges, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, eventlist, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, nevents, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, timeout, timespec, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_EV_SET, 0, 7, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, kevp, kevent, 0)
	ZEND_ARG_TYPE_INFO(0, ident, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, filter, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, fflags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, udata, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_EV_SET64, 0, 9, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, kevp, kevent64_s, 0)
	ZEND_ARG_TYPE_INFO(0, ident, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, filter, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, fflags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, udata, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ext0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ext1, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_kqueue_errno arginfo_kqueue

ZEND_FUNCTION(kqueue);
ZEND_FUNCTION(kevent);
ZEND_FUNCTION(kevent64);
ZEND_FUNCTION(EV_SET);
ZEND_FUNCTION(EV_SET64);
ZEND_FUNCTION(kqueue_errno);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(kqueue, arginfo_kqueue)
	ZEND_FE(kevent, arginfo_kevent)
	ZEND_FE(kevent64, arginfo_kevent64)
	ZEND_FE(EV_SET, arginfo_EV_SET)
	ZEND_FE(EV_SET64, arginfo_EV_SET64)
	ZEND_FE(kqueue_errno, arginfo_kqueue_errno)
	ZEND_FE_END
};

static void register_kqueue_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("EVFILT_READ", EVFILT_READ, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_WRITE", EVFILT_WRITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_AIO", EVFILT_AIO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_VNODE", EVFILT_VNODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_PROC", EVFILT_PROC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_SIGNAL", EVFILT_SIGNAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_TIMER", EVFILT_TIMER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_MACHPORT", EVFILT_MACHPORT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_FS", EVFILT_FS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_USER", EVFILT_USER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_VM", EVFILT_VM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_EXCEPT", EVFILT_EXCEPT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_SYSCOUNT", EVFILT_SYSCOUNT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVFILT_THREADMARKER", EVFILT_THREADMARKER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("KEVENT_FLAG_NONE", KEVENT_FLAG_NONE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("KEVENT_FLAG_IMMEDIATE", KEVENT_FLAG_IMMEDIATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("KEVENT_FLAG_ERROR_EVENTS", KEVENT_FLAG_ERROR_EVENTS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_ADD", EV_ADD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_DELETE", EV_DELETE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_ENABLE", EV_ENABLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_DISABLE", EV_DISABLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_ONESHOT", EV_ONESHOT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_CLEAR", EV_CLEAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_RECEIPT", EV_RECEIPT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_DISPATCH", EV_DISPATCH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_UDATA_SPECIFIC", EV_UDATA_SPECIFIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_DISPATCH2", EV_DISPATCH2, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_VANISHED", EV_VANISHED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_SYSFLAGS", EV_SYSFLAGS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_FLAG0", EV_FLAG0, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_FLAG1", EV_FLAG1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_EOF", EV_EOF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_ERROR", EV_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_POLL", EV_POLL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_OOBAND", EV_OOBAND, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_TRIGGER", NOTE_TRIGGER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_FFNOP", NOTE_FFNOP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_FFAND", NOTE_FFAND, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_FFOR", NOTE_FFOR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_FFCOPY", NOTE_FFCOPY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_FFCTRLMASK", NOTE_FFCTRLMASK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_FFLAGSMASK", NOTE_FFLAGSMASK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_LOWAT", NOTE_LOWAT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_OOB", NOTE_OOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_DELETE", NOTE_DELETE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_WRITE", NOTE_WRITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXTEND", NOTE_EXTEND, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_ATTRIB", NOTE_ATTRIB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_LINK", NOTE_LINK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_RENAME", NOTE_RENAME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_REVOKE", NOTE_REVOKE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_NONE", NOTE_NONE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_FUNLOCK", NOTE_FUNLOCK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_LEASE_DOWNGRADE", NOTE_LEASE_DOWNGRADE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_LEASE_RELEASE", NOTE_LEASE_RELEASE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXIT", NOTE_EXIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_FORK", NOTE_FORK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXEC", NOTE_EXEC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_REAP", NOTE_REAP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_SIGNAL", NOTE_SIGNAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXITSTATUS", NOTE_EXITSTATUS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXIT_DETAIL", NOTE_EXIT_DETAIL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_PDATAMASK", NOTE_PDATAMASK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_PCTRLMASK", NOTE_PCTRLMASK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXIT_REPARENTED", NOTE_EXIT_REPARENTED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXIT_DETAIL_MASK", NOTE_EXIT_DETAIL_MASK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXIT_DECRYPTFAIL", NOTE_EXIT_DECRYPTFAIL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXIT_MEMORY", NOTE_EXIT_MEMORY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_EXIT_CSERROR", NOTE_EXIT_CSERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_VM_PRESSURE", NOTE_VM_PRESSURE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_VM_PRESSURE_TERMINATE", NOTE_VM_PRESSURE_TERMINATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_VM_PRESSURE_SUDDEN_TERMINATE", NOTE_VM_PRESSURE_SUDDEN_TERMINATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_VM_ERROR", NOTE_VM_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_SECONDS", NOTE_SECONDS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_USECONDS", NOTE_USECONDS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_NSECONDS", NOTE_NSECONDS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_ABSOLUTE", NOTE_ABSOLUTE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_LEEWAY", NOTE_LEEWAY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_CRITICAL", NOTE_CRITICAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_BACKGROUND", NOTE_BACKGROUND, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_MACH_CONTINUOUS_TIME", NOTE_MACH_CONTINUOUS_TIME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_MACHTIME", NOTE_MACHTIME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_TRACK", NOTE_TRACK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_TRACKERR", NOTE_TRACKERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NOTE_CHILD", NOTE_CHILD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EACCES", EACCES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EBADF", EBADF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EFAULT", EFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EINTR", EINTR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EINVAL", EINVAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EMFILE", EMFILE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENFILE", ENFILE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENOENT", ENOENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENOMEM", ENOMEM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ESRCH", ESRCH, CONST_PERSISTENT);
}

static zend_class_entry *register_class_kevent(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "kevent", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	zval property_ident_default_value;
	ZVAL_LONG(&property_ident_default_value, 0);
	zend_string *property_ident_name = zend_string_init("ident", sizeof("ident") - 1, 1);
	zend_declare_typed_property(class_entry, property_ident_name, &property_ident_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_ident_name);

	zval property_filter_default_value;
	ZVAL_LONG(&property_filter_default_value, 0);
	zend_string *property_filter_name = zend_string_init("filter", sizeof("filter") - 1, 1);
	zend_declare_typed_property(class_entry, property_filter_name, &property_filter_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_filter_name);

	zval property_flags_default_value;
	ZVAL_LONG(&property_flags_default_value, 0);
	zend_string *property_flags_name = zend_string_init("flags", sizeof("flags") - 1, 1);
	zend_declare_typed_property(class_entry, property_flags_name, &property_flags_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_flags_name);

	zval property_fflags_default_value;
	ZVAL_LONG(&property_fflags_default_value, 0);
	zend_string *property_fflags_name = zend_string_init("fflags", sizeof("fflags") - 1, 1);
	zend_declare_typed_property(class_entry, property_fflags_name, &property_fflags_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_fflags_name);

	zval property_data_default_value;
	ZVAL_LONG(&property_data_default_value, 0);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_data_name);

	zval property_udata_default_value;
	ZVAL_LONG(&property_udata_default_value, 0);
	zend_string *property_udata_name = zend_string_init("udata", sizeof("udata") - 1, 1);
	zend_declare_typed_property(class_entry, property_udata_name, &property_udata_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_udata_name);

	return class_entry;
}

static zend_class_entry *register_class_kevent64_s(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "kevent64_s", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	zval property_ident_default_value;
	ZVAL_LONG(&property_ident_default_value, 0);
	zend_string *property_ident_name = zend_string_init("ident", sizeof("ident") - 1, 1);
	zend_declare_typed_property(class_entry, property_ident_name, &property_ident_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_ident_name);

	zval property_filter_default_value;
	ZVAL_LONG(&property_filter_default_value, 0);
	zend_string *property_filter_name = zend_string_init("filter", sizeof("filter") - 1, 1);
	zend_declare_typed_property(class_entry, property_filter_name, &property_filter_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_filter_name);

	zval property_flags_default_value;
	ZVAL_LONG(&property_flags_default_value, 0);
	zend_string *property_flags_name = zend_string_init("flags", sizeof("flags") - 1, 1);
	zend_declare_typed_property(class_entry, property_flags_name, &property_flags_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_flags_name);

	zval property_fflags_default_value;
	ZVAL_LONG(&property_fflags_default_value, 0);
	zend_string *property_fflags_name = zend_string_init("fflags", sizeof("fflags") - 1, 1);
	zend_declare_typed_property(class_entry, property_fflags_name, &property_fflags_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_fflags_name);

	zval property_data_default_value;
	ZVAL_LONG(&property_data_default_value, 0);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_data_name);

	zval property_udata_default_value;
	ZVAL_LONG(&property_udata_default_value, 0);
	zend_string *property_udata_name = zend_string_init("udata", sizeof("udata") - 1, 1);
	zend_declare_typed_property(class_entry, property_udata_name, &property_udata_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_udata_name);

	zval property_ext_default_value;
	ZVAL_UNDEF(&property_ext_default_value);
	zend_string *property_ext_name = zend_string_init("ext", sizeof("ext") - 1, 1);
	zend_declare_typed_property(class_entry, property_ext_name, &property_ext_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_ext_name);

	return class_entry;
}

static zend_class_entry *register_class_timespec(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "timespec", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	zval property_tv_sec_default_value;
	ZVAL_LONG(&property_tv_sec_default_value, 0);
	zend_string *property_tv_sec_name = zend_string_init("tv_sec", sizeof("tv_sec") - 1, 1);
	zend_declare_typed_property(class_entry, property_tv_sec_name, &property_tv_sec_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_tv_sec_name);

	zval property_tv_nsec_default_value;
	ZVAL_LONG(&property_tv_nsec_default_value, 0);
	zend_string *property_tv_nsec_name = zend_string_init("tv_nsec", sizeof("tv_nsec") - 1, 1);
	zend_declare_typed_property(class_entry, property_tv_nsec_name, &property_tv_nsec_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_tv_nsec_name);

	return class_entry;
}
