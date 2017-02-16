After being disabled, plugin is not listed anymore
  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin" > /dev/null 2>&1
  $ $ZPM disable "$ZPM_TEST_REPOS/example-plugin" > /dev/null 2>&1
  $ $ZPM list
  [ZPM] Nothing to show.
  [1]

Plugin directory is not removed after disable
  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin" > /dev/null 2>&1
  $ $ZPM disable "$ZPM_TEST_REPOS/example-plugin" > /dev/null 2>&1
  $ ls $ZPM_TEST_REPOS
  example-plugin
  .* (re)

Plugin can be properly enabled/installed after "disable"
  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin" > /dev/null 2>&1
  $ $ZPM disable "$ZPM_TEST_REPOS/example-plugin"> /dev/null 2>&1 
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin" > /dev/null 2>&1
  $ $ZPM list
  .*example-plugin@.* (re)
