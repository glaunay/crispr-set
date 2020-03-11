from distutils.core import setup, Extension

module = Extension('twobits',
                     sources = ['./two_bits_encoding.c', 'twobitsmodule.c'],
                     extra_compile_args=['-pedantic', '-std=c99'],
                     include_dirs = ['./include']
                     )

setup (name = 'twobits',
       author_email = 'pitooon@gmail.com',
	version = '1.4',
       #define_macros = [('MAJOR_VERSION', '1'),
       #                              ('MINOR_VERSION', '0')],
       description = 'Encodind/decoding/Truncating k-mers in 2bits-per-base ',
       ext_modules = [module])

