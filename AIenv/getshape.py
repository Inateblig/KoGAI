import os
from stable_baselines3 import PPO

model = PPO.load(f"models/20220919_230231/model0.zip", print_system_info=True)
print(model.policy)
