from setuptools import find_packages, setup

package_name = 'pilot_6axis'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='lemonx',
    maintainer_email='dudzinski.patryk.02@gmail.com',
    description='6 axis remote Controler',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [ 'rc_node = pilot_6axis.pilot_6axis:main'
        ],
    },
)
