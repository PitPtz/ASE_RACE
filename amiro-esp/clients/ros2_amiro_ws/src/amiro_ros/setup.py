from setuptools import setup

package_name = 'amiro_ros'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Christopher Niemann',
    maintainer_email='cniemann@techfak.uni-bielefeld.de',
    description='AMiRo ROS 2 Interface',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'amiro = amiro_ros.amiro_node:main',
        ],
    },
)
