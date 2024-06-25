from setuptools import setup, find_namespace_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

name = "amiro-remote"
version = "0.0.1"

cmdclass = {}

try:
    from sphinx.setup_command import BuildDoc
    cmdclass['build_sphinx'] = BuildDoc
except ImportError:
    print('WARNING: Sphinx not available, not building docs')

setup(
    name=name,
    version=version,
    author="Christopher Niemann",
    author_email="cniemann@techfak.de",
    description="Receive and send data of an AMiRo remotely",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://gitlab.ub.uni-bielefeld.de/ks-nano/amiro/hardware/espamiro/-/tree/main",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    packages=find_namespace_packages(where="src"),
    package_dir={"": "src"},
    entry_points={
        "console_scripts": [
            "amiro-remote=amiro_remote.cli:main",
        ]
    },
    cmdclass=cmdclass,
    python_requires=">=3.7",
    install_requires=[
        "numpy",
        "matplotlib",
        "basyx-python-sdk==0.2.2",
        "ml4proflow-mods-basyx @ git+https://gitlab.ub.uni-bielefeld.de/ml4proflow/ml4proflow-basyx.git",
    ],
    extras_require={
        "tests": ["pytest"],
        "docs": ["sphinx", "sphinx-rtd-theme", "recommonmark"],
    },
    command_options={
        'build_sphinx': {
            'project': ('setup.py', name),
            'version': ('setup.py', version),
            'release': ('setup.py', version),
            'source_dir': ('setup.py', 'docs/source/'),
            'build_dir': ('setup.py', 'docs/build/')
        }
    },
)
