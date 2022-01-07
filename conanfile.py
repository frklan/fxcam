from conans import ConanFile

class fxcamProject(ConanFile):
    # Note: options are copied from CMake boolean options.
    # When turned off, CMake sometimes passes them as empty strings.
    options = {

    }
    name = "fxcam"
    version = "0.1"
    requires = (
        "spdlog/1.9.2",
        "sdl/2.0.18",
        "opencv/4.5.3",
        "imgui/1.85"
        )
    generators = "cmake", "gcc", "txt", "cmake_find_package"
    
    def configure(self):
        
        self.options["opencv"].with_ffmpeg = False
        self.options["opencv"].contrib = False
        self.options["opencv"].dnn = True
        self.options["opencv"].fPIC = True
        self.options["opencv"].neon = False
        self.options["opencv"].parallel = False
        self.options["opencv"].with_ade = False
        self.options["opencv"].with_eigen = False
        self.options["opencv"].with_ffmpeg = False
        self.options["opencv"].with_jpeg2000 = False
        self.options["opencv"].with_openexr = False
        self.options["opencv"].with_png = False
        self.options["opencv"].with_quirc = False
        self.options["opencv"].with_tiff = False
        self.options["opencv"].with_webp = False
        self.options["opencv"].detect_cpu_baseline = True
        self.options["opencv"].parallel = False

        
        