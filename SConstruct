
env = Environment()

# output to the build dir, from source in the src dir
env.VariantDir('build', 'src', duplicate=0)

# output a compilation database
env.Tool('compilation_db')
env.CompilationDatabase()

env.Program('shittypong', Glob('build/*.cpp'), LIBS=['GLEW', 'GL', 'glfw'], CPPPATH='./include')

