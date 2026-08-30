use std::ffi::c_char;

#[no_mangle]
pub unsafe extern "C" fn uox3_mozjs_version() -> *const c_char {
    mozjs_sys::jsapi::JS_GetImplementationVersion()
}
