{ pkgs, ... }:

{
  # https://devenv.sh/packages/
  packages = [
  	pkgs.cmake    # Adding cmake
  	pkgs.gcc13    # Adding gcc 13
  	pkgs.openblas # Adding OpenBLAS package
    pkgs.SDL2     # Adding SDL2 package
  ];

  languages.cplusplus = {
  	enable = true;
  };
  env = {
      "OPENBLAS" = "1";
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
