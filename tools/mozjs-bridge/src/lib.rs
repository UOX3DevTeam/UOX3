use std::ffi::c_char;

/// Keeps mozjs-sys and its Rust support libraries in the C++ link archive.
#[no_mangle]
pub unsafe extern "C" fn uox3_mozjs_version() -> *const c_char {
    mozjs_sys::jsapi::JS_GetImplementationVersion()
}
