project('windows', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

epoxy_dep = dependency('epoxy', version:'>=1.0')

executable('windows',
           'opengl_window.c',
           install : true,
           dependencies: [epoxy_dep])
