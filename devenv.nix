{ pkgs, ... }:

{
  # https://devenv.sh/packages/
  packages = [
  	pkgs.cmake    # Adding cmake
  	pkgs.gcc13    # Adding gcc 13
  	pkgs.openblas # Adding OpenBLAS package
  ];

  languages.cplusplus = {
  	enable = true;
  };
  languages.python = {
  	enable = true;
  	#version = "3.9.1";
  	venv = {
  	     enable = true;
  	     requirements = ''
  			numpy --no-binary :all:
  			pandas
  			matplotlib
  			imageio
              requests
              pandas
              matplotlib
              seaborn
  			black
  		 '';
  	};
  };
  env = {
      "OPENBLAS" = "1";
      "NUMPY_BLAS_LIB" = "openblas";
      "NUMPY_LAPACK_LIB" = "openblas";
  	"CXX"="${pkgs.gcc13}/bin/g++";
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
