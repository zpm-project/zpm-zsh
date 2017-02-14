After being disabled, plugin is not listed anymore
  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin" > /dev/null
  $ $ZPM disable "$ZPM_TEST_REPOS/example-plugin" > /dev/null
  $ $ZPM list
  Nothing to show.
  [1]

Plugin directory is not removed after disable
  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin" > /dev/null
  $ $ZPM disable "$ZPM_TEST_REPOS/example-plugin" > /dev/null
  $ ls $ZPM_TEST_REPOS
  example-plugin
  .* (re)

Plugin can be properly enabled/installed after "disable"
  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin" > /dev/null
  $ $ZPM disable "$ZPM_TEST_REPOS/example-plugin" > /dev/null
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin" > /dev/null
  $ $ZPM list
  .*example-plugin@.* (re)
