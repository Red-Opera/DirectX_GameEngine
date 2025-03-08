import numpy as np
from matplotlib import pyplot as plt

data = np.load('../../Temp/ShadowMap0.npy')
plt.imshow(data)
plt.axis('off')
plt.show()