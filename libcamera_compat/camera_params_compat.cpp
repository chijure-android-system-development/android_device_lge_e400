/*
 * CameraParameters statics that the stock LG libcamera.so (installed as
 * libcamera.e400.so) imports from libcamera_client and that CM7 does not
 * define. The Gingerbread linker only resolves a dlopen()ed library's symbols
 * in its own dependencies, so they must live in libcamera_client; they are
 * compiled into it through BOARD_CAMERA_CLIENT_EXTRA_SRC.
 *
 * The values are the ones exported by the stock libcamera_client.so.
 */

#include <camera/CameraParameters.h>

namespace android {

const char CameraParameters::KEY_DENOISE[] = "denoise";
const char CameraParameters::KEY_SUPPORTED_DENOISE[] = "denoise-values";
const char CameraParameters::DENOISE_OFF[] = "denoise-off";
const char CameraParameters::DENOISE_ON[] = "denoise-on";
const char CameraParameters::KEY_MEMORY_COLOR_ENHANCEMENT[] = "mce";
const char CameraParameters::KEY_SUPPORTED_MEM_COLOR_ENHANCE_MODES[] = "mce-values";
const char CameraParameters::MCE_ENABLE[] = "enable";
const char CameraParameters::MCE_DISABLE[] = "disable";
const char CameraParameters::KEY_REDEYE_REDUCTION[] = "redeye-reduction";
const char CameraParameters::KEY_SUPPORTED_REDEYE_REDUCTION[] = "redeye-reduction-values";
const char CameraParameters::REDEYE_REDUCTION_ENABLE[] = "enable";
const char CameraParameters::REDEYE_REDUCTION_DISABLE[] = "disable";
const char CameraParameters::KEY_VIDEO_HIGH_FRAME_RATE[] = "video-hfr";
const char CameraParameters::KEY_SUPPORTED_VIDEO_HIGH_FRAME_RATE_MODES[] = "video-hfr-values";
const char CameraParameters::KEY_SUPPORTED_HFR_SIZES[] = "hfr-size-values";
const char CameraParameters::VIDEO_HFR_OFF[] = "off";
const char CameraParameters::VIDEO_HFR_2X[] = "60";
const char CameraParameters::VIDEO_HFR_3X[] = "90";
const char CameraParameters::VIDEO_HFR_4X[] = "120";
const char CameraParameters::PIXEL_FORMAT_YV12[] = "yv12";

}; // namespace android
