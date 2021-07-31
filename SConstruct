env = Environment()
env.VariantDir('build', 'src', duplicate=0)     # output to the build dir, from source in the src dir
env.Program('shittypong', Glob('build/*.cpp'), LIBS=['GLEW', 'GL', 'glfw'], CPPPATH='./include')
