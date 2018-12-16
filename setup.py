import os
import re
import sys
import glob
import sysconfig
import platform
import subprocess

from distutils.version import LooseVersion
from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext
from shutil import copyfile, copymode
from setuptools import find_packages, setup, Command


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)',
                                         out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def copy_test_file(self, ext, src_file):

        '''
        Copy ``src_file`` to `tests/bin` directory, ensuring parent directory
        exists. Messages like `creating directory /path/to/package` and
        `copying directory /src/path/to/package -> path/to/package` are
        displayed on standard output. Adapted from scikit-build.
        '''
        # Create directory if needed
        dest_dir = os.path.join(os.path.dirname(
            os.path.abspath(__file__)), ext.name, 'tests', 'bin')
        if dest_dir != "" and not os.path.exists(dest_dir):
            print("creating directory {}".format(dest_dir))
            os.makedirs(dest_dir)

        # Copy file
        dest_file = os.path.join(dest_dir, os.path.basename(src_file))
        print("copying {} -> {}".format(src_file, dest_file))
        copyfile(src_file, dest_file)
        copymode(src_file, dest_file)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DCMAKE_DEBUG_POSTFIX=""']

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(
                cfg.upper(),
                extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)
        # Copy *_test file to tests directory
        test_bin = os.path.join(self.build_temp, 'kgekit', 'kgekit_test')
        self.copy_test_file(ext, test_bin)
        print()  # Add an empty line for cleaner output


class CleanCommand(Command):
    """Custom clean command to tidy up the project root."""
    user_options = []
    def initialize_options(self):
        pass
    def finalize_options(self):
        pass
    def run(self):
        os.system('rm -vrf ./build ./dist ./*.pyc ./*.tgz ./*.egg-info')


setup(name='kgekit',
      version='0.4.0',
      description='knowledge representation tools',
      long_description=open("README.md", "r").read(),
      long_description_content_type="text/markdown",
      url='http://github.com/fantasticfears/kgekit',
      author='Erick Guan',
      author_email='fantasticfears@gmail.com',
      license='GPLv2',
      packages=find_packages(),
      install_requires=[
        'protobuf>=3',
        'numpy>=1.10'
      ],
      cmdclass=dict(build_ext=CMakeBuild, clean=CleanCommand),
      ext_modules=[CMakeExtension('kgekit')],
      test_suite='tests',
      zip_safe=False,
      classifiers=[
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',

        # Indicate who your project is intended for
        'Intended Audience :: Developers',
        'Programming Language :: Python :: 3',
      ],
)
