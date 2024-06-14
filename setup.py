# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#  PCMPy - Lib
#  Python Extension for  Oracle BRM
#  Copyright Cassio Jemael 2024
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 


from distutils.core import setup, Extension
import os

PIN_HOME =  os.getenv('PIN_HOME')

# Ajuste estes caminhos conforme necessario para sua instalacao Oracle BRM
# include_dirs = ['/opt/portal/7.5/include', "/usr/include/libxml2"]
include_dirs = [ (PIN_HOME + '/include'), '/usr/include/libxml2']
#library_dirs = ['/opt/portal/7.5/lib']
library_dirs = [(PIN_HOME + '/lib')]
libraries = ['portal64', 'pcmext64', 'xml2']





# extra_compile_args=['-O2', '-Wall'],  # Adicionando flags de compilacao
# extra_link_args=['-Wl,-rpath,/path/to/oracle/brm/lib']  # Adicionando flags de linkagem
link_flags = ['-Wno-address', '-shared', '-Wno-unused-variable', '-lm', '-fPIC']
compile_flags = ['-g',  '-fPIC', '-Wno-unused-variable', '-Wno-address', '-Wmaybe-uninitialized']

module = Extension('PCMPy',
                   sources=['bind.c', 
                            'pcm_py_main.c',
                            'pcm_py_log.c', 
                            'pcm_py_poid_funcions.c', 
                            'pcm_py_fields_ops.c', 
                            'pcm_py_functions.c',
                            'pcm_py_pinconf.c',
                            'pcm_py_transactions.c',
                            'pcm_py_constants.c'
                            ],
                   include_dirs=include_dirs,
                   library_dirs=library_dirs,
                   libraries=libraries,
                   extra_compile_args=compile_flags,
                   extra_link_args=link_flags)

setup(name='PCMPy',
      version='1.0',
      description='Oracle BRM PCM Lib for Python',
      ext_modules = [module]
      )
