# To learn more about how to use Nix to configure your environment
# see: https://developers.google.com/idx/guides/customize-idx-env
{ pkgs, ... }: {
  # Which nixpkgs channel to use.
  channel = "stable-24.05"; # or "unstable"
  # Use https://search.nixos.org/packages to find packages
  packages = [
    pkgs.gcc
    pkgs.zsh
    pkgs.git
    pkgs.nodejs
    pkgs.sudo
    pkgs.astyle
    pkgs.git-credential-manager
  ];
  # Sets environment variables in the workspace
  idx = {
    # Search for the extensions you want on https://open-vsx.org/ and use "publisher.id"
    extensions = [
      # "vscodevim.vim"
      "13xforever.language-x86-64-assembly"
      "bbenoist.Doxygen"
      "cheshirekow.cmake-format"
      "cschlosser.doxdocgen"
      "franneck94.c-cpp-runner"
      "franneck94.vscode-c-cpp-config"
      "franneck94.vscode-c-cpp-dev-extension-pack"
      "jeff-hykin.better-cpp-syntax"
      "PROxZIMA.sweetdracula"
      "twxs.cmake"
      "vadimcn.vscode-lldb"
      "danielpinto8zz6.c-cpp-compile-run"
      "ms-vscode.cpptools-themes"
    ];
    # Enable previews
    previews = {
      enable = true;
      previews = {
        # web = {
        #   # Example: run "npm run dev" with PORT set to IDX's defined port for previews,
        #   # and show it in IDX's web preview panel
        #   command = ["npm" "run" "dev"];
        #   manager = "web";
        #   env = {
        #     # Environment variables to set for your server
        #     PORT = "$PORT";
        #   };
        # };
      };
    };
    # Workspace lifecycle hooks
    workspace = {
      # Runs when a workspace is first created
      onCreate = {
        # Example: install JS dependencies from NPM
        # npm-install = "npm install";
        # Open editors for the following files by default, if they exist:
        default.openFiles = [ ".idx/dev.nix" "README.md" ];
      };
      # Runs when the workspace is (re)started
      onStart = {
        # Example: start a background task to watch and re-build backend code
        # watch-backend = "npm run watch-backend";
      };
    };
  };
}
