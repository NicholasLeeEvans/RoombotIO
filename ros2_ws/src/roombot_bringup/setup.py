from setuptools import find_packages, setup
from glob import glob

package_name = 'roombot_bringup'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch', glob('launch/*.launch.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Evans',
    maintainer_email='perfectchumbles@gmail.com',
    description='Roombot nodes and launch files',
    license='MIT',
    entry_points={
        'console_scripts': [
            'circle_odom = roombot_bringup.circle_odom:main',
        ],
    },
)
