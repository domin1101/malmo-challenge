
from __future__ import absolute_import

from malmopy.model import BaseModel
import tensorflow as tf
import json
import numpy as np

# Describes a fully connected neural network using tensorflow
class MLPTensor:

    # Imports the neural network from the given .json file
    # Activation functions: ReLU (hidden), sigmoid (output)
    def __init__(self, path):

        # Read data
        with open(path) as data_file:
            data = json.load(data_file)

        # Extract input and output size from json structure
        self.input_shape = len(data[0]['weights'][0])
        self.output_shape = len(data[-1]['weights'])

        with tf.name_scope('NN'):

            # Create placeholder for input value
            self.x = tf.placeholder(tf.float32, shape=[None, self.input_shape])

            # Declare parameter lists
            self.W = []
            self.b = []
            self.y = []

            # Will always contain the output tensor of the previous layer
            prev_y = self.x
            for layer_data in data[:-1]:
                # Extract layer size out of weights
                hidden_layer_size = len(layer_data['weights'])
                # Create layer weights
                self.W.append(self.weight_variable(layer_data['weights']))
                # Create layer bias
                self.b.append(self.bias_variable(layer_data['bias']))
                # Create layer output
                self.y.append(tf.nn.relu(tf.matmul(prev_y, self.W[-1]) + self.b[-1]))

                # Set new layer as new previous layer
                prev_y = self.y[-1]

            # Create all parameters for output layer
            self.W.append(self.weight_variable(data[-1]['weights']))
            self.b.append(self.bias_variable(data[-1]['bias']))
            self.y.append(tf.cast(tf.greater(tf.matmul(prev_y, self.W[-1]) + self.b[-1], 0), tf.float32))

        # Create and initialize session
        self.sess = tf.Session()
        self.sess.run(tf.global_variables_initializer())

    # Executes the neural network
    def __call__(self, x):
        ret = self.sess.run(self.y[-1], feed_dict={self.x: x})
        return ret

    # Creates a new matrix used for weights with the given data as initial value
    def weight_variable(self, data):
        initial = np.asarray(data).transpose()
        return tf.Variable(initial, name="weights", dtype=tf.float32)

    # Creates a new vector used for bias with the given data as initial value
    def bias_variable(self, data):
        initial = np.asarray(data)
        return tf.Variable(initial, name="bias", dtype=tf.float32)

# A simple neural network
class NeuralNetwork(BaseModel):

    # Creates a neural network with the given model
    def __init__(self, model):
        super(NeuralNetwork, self).__init__(model.input_shape, model.output_shape)

        self._gpu_device = None
        self._model = model

    # Runs the neural network
    def evaluate(self, environment):
        return self._model(environment)



