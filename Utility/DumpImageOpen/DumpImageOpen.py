import numpy as np
from matplotlib import pyplot as plt

data = np.load('../../Temp/Temp.npy')
plt.imshow(data)
plt.axis('off')
plt.show()