# vim: set ts=2 sw=2 expandtab:

Can install multiple plugins under the same user.

  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin"
  \[ZPM\] Installing .*/example-plugin... Done. (re)
  $ $ZPM "$ZPM_TEST_REPOS/example-theme"
  \[ZPM\] Installing .*/example-theme... Done. (re)
  $ ls $ZPM_TEST_REPOS
  example-plugin (re)
  example-theme (re)

Avoid installing plugin twice
  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin"2<>11 > /dev/null
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin"
  \[ZPM\] Plugin ".*/example-plugin" already installed. (re)
  [1]
