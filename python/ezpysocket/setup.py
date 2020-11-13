import setuptools
from ezpysocket import __version__

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="ezpysocket",
    version=__version__,
    author="Aditya Divakaran",
    author_email="adi.develops@gmail.com",
    description="Cpp Python socket abstraction library that attempts to simplify TCP/IP socket communication.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/Aditya-Diva/Ez-Cpp-Python-Socket",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
    install_requires=['numpy'],
)   