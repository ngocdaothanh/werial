#include <ruby.h>
#include <windows.h>
#include <winbase.h>

#define WERIAL_BUF_SIZE 9999

VALUE werial_open(VALUE self, VALUE port) {
	VALUE  rhandle;
	HANDLE handle;

	// Close any open handle before opening a new one
	rhandle = rb_iv_get(self, "@handle");
	if (!NIL_P(rhandle)) {
		werial_close(self);
	}

	handle = CreateFileA(
		RSTRING_PTR(port),
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	rhandle = INT2NUM((int) handle);
	rb_iv_set(self, "@handle", rhandle);

	return (handle == INVALID_HANDLE_VALUE)? Qfalse : Qtrue;
}

VALUE werial_close(VALUE self) {
	VALUE  rhandle;
	HANDLE handle;

	rhandle = rb_iv_get(self, "@handle");
	if (!NIL_P(rhandle)) {
		handle = (HANDLE) NUM2INT(rhandle);
		if (handle != INVALID_HANDLE_VALUE) {
			CloseHandle(handle);
		}
		rb_iv_set(self, "@handle", Qnil);
	}

	return Qnil;
}

VALUE werial_data(VALUE self) {
	VALUE  rhandle;
	HANDLE handle;
	char buf[WERIAL_BUF_SIZE];
	int read_size;

	rhandle = rb_iv_get(self, "@handle");
	if (NIL_P(rhandle)) {
		return Qnil;
	}

	handle = (HANDLE) NUM2INT(rhandle);
	if (handle == INVALID_HANDLE_VALUE) {
		return Qnil;
	}

	if (!ReadFile(handle, &buf, WERIAL_BUF_SIZE, &read_size, NULL) || read_size <= 0) {
		return Qnil;
	}

	return rb_str_new(buf, read_size);
}

void Init_werial() {
	VALUE Werial = rb_define_class("Werial", rb_cObject);
	rb_define_method(Werial, "open", werial_open, 1);
	rb_define_method(Werial, "close", werial_close, 0);
	rb_define_method(Werial, "data", werial_data, 0);
}
