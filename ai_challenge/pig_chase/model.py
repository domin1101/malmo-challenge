
from __future__ import absolute_import

import itertools

from malmopy.model import BaseModel
import tensorflow as tf

class MLPTensor:
    def __init__(self, in_shape, output_shape, hidden_layer_sizes):
        self.input_shape = in_shape
        self.output_shape = output_shape

        self.x = tf.placeholder(tf.float32, shape=[None, in_shape])

        self.W = []
        self.mut_W = []
        self.b = []
        self.mut_b = []
        self.y = []

        prev_hidden_layer_size = in_shape
        prev_y = self.x
        for hidden_layer_size in hidden_layer_sizes:
            self.W.append(self.weight_variable([prev_hidden_layer_size, hidden_layer_size]))
            self.mut_W.append(self.mutation_strength_variable([prev_hidden_layer_size, hidden_layer_size]))
            self.b.append(self.bias_variable([hidden_layer_size]))
            self.mut_b.append(self.mutation_strength_variable([hidden_layer_size]))
            prev_hidden_layer_size = hidden_layer_size

            self.y.append(tf.nn.relu(tf.matmul(prev_y, self.W[-1]) + self.b[-1]))
            prev_y = self.y[-1]

        self.W.append(self.weight_variable([prev_hidden_layer_size, output_shape]))
        self.mut_W.append(self.mutation_strength_variable([prev_hidden_layer_size, output_shape]))
        self.b.append(self.bias_variable([output_shape]))
        self.mut_b.append(self.mutation_strength_variable([output_shape]))
        self.y.append(tf.nn.sigmoid(tf.matmul(prev_y, self.W[-1]) + self.b[-1]))

        self.mutationStrengthChangeSpeed = tf.constant(1.6)
        self.mutationStrengthMin = tf.constant(0.000001)
        self.mutationStrengthMax = tf.constant(50.0)

        self.sess = tf.Session()
        self.sess.run(tf.global_variables_initializer())

    def __call__(self, x):
        return self.sess.run(self.y[-1], feed_dict={self.x: x})

    def weight_variable(self, shape):
        initial = tf.truncated_normal(shape, stddev=0.1)
        return tf.Variable(initial)

    def bias_variable(self, shape):
        initial = tf.constant(0.1, shape=shape)
        return tf.Variable(initial)

    def mutation_strength_variable(self, shape):
        initial = tf.constant(0.2, shape=shape)
        return tf.Variable(initial)

    def copyWeightsFrom(self, other):
        # copy = self.W_1.assign(other.W_1), self.W_2.assign(other.W_2), self.b_1.assign(other.b_1), self.b_2.assign(other.b_2)
        # self.sess.run(copy)
        pass

    def mutate(self):

        for mut_W in self.mut_W:
            mut_W *= tf.exp(self.mutationStrengthChangeSpeed * tf.random_normal(mut_W.shape))
            mut_W = tf.minimum(self.mutationStrengthMax, tf.maximum(self.mutationStrengthMin, mut_W))
            self.sess.run(mut_W)

        for W, mut_W in itertools.izip(self.W, self.mut_W):
            W += mut_W * tf.random_normal(mut_W.shape)




class NeuralNetwork(BaseModel):
    def __init__(self, model, device_id=-1):

        assert isinstance(model, MLPTensor), \
            'model should inherit from ChainerModel'

        super(NeuralNetwork, self).__init__(model.input_shape, model.output_shape)

        self._gpu_device = None

        # Setup model and target network
        self._model = model

    def evaluate(self, environment):
        environment = environment.ravel()[None, :]

        return self._model(environment)

    def copyWeightsFrom(self, neuralNetwork):
        self._model.copyWeightsFrom(neuralNetwork._model)

    def mutate(self):
        self._model.mutate()



