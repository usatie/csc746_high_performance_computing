{ pkgs, ... }:

{
  # https://devenv.sh/packages/
  packages = [
  	pkgs.cmake    # Adding cmake
  	pkgs.gcc13    # Adding gcc 13
  	pkgs.openblas # Adding OpenBLAS package
    pkgs.SDL2     # Adding SDL2 package
	pkgs.ffmpeg   # Adding ffmpeg package
  ];

  languages.cplusplus = {
  	enable = true;
  };
  languages.python = {
  	enable = true;
  	version = "3.9.1";
  	venv = {
  		 enable = true;
  		 requirements = ''
  			numpy --no-binary=numpy
  			pandas
  			matplotlib
  			seaborn
  		 '';
  	};
  };
  env = {
      "OPENBLAS" = "1";
      "NUMPY_BLAS_LIB" = "openblas";
      "NUMPY_LAPACK_LIB" = "openblas";
  	  "CXX"="${pkgs.gcc13}/bin/g++";
	  "SDL2_CONFIG"="${pkgs.SDL2}/bin/sdl2-config";
	  "SDL2_PATH"="${pkgs.SDL2}";
  };

  # https://devenv.sh/pre-commit-hooks/
  pre-commit.hooks = {
    # lint shell scripts
    shellcheck.enable = true;
	# format Python code
	black.enable = true;
	# format C++ code
	clang-format.enable = true;
  };
}
