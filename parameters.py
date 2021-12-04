from generate_configs import generate_reservoirs, generate_parameters

p1 = {
   'process': 'Erosion',
   'parameter': 'alpha',
   'log': True,
   'vmin': 1e-5,
   'vmax': 1e-2
 }

"""
p2 = {
   'process': 'Volcanism',
   'parameter': 'F_arc',
   'log': True,
   'vmin': 0.1,
   'vmax': 10
 }
"""
generate_parameters([p1], n=100, default="./new_config.yaml")
