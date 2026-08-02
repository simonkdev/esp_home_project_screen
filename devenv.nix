{
  pkgs,
  lib,
  config,
  inputs,
  ...
}:
{
  packages = with pkgs; [
    git
    platformio
  ];
  scripts.upload.exec = ''
    pio run --target upload
  '';
}
