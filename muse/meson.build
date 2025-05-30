boost_dep = dependency('boost')

inc = include_directories('include', '.')
subdir('core')
library('muse', include_directories: inc, link_with: libcore, dependencies: boost_dep)