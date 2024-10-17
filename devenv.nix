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
  env = {
      "OPENBLAS" = "1";
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
