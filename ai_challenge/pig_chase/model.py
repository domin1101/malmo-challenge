
from __future__ import absolute_import

from malmopy.model import BaseModel
import tensorflow as tf

class MLPTensor:
    def __init__(self, in_shape, output_shape, hidden_layer_size=512):
        self.input_shape = in_shape
        self.output_shape = output_shape

        self.x = tf.placeholder(tf.float32, shape=[None, in_shape])

        self.W_1 = self.weight_variable([in_shape, hidden_layer_size])
        self.b_1 = self.bias_variable([hidden_layer_size])

        self.W_2 = self.weight_variable([hidden_layer_size, output_shape])
        self.b_2 = self.bias_variable([output_shape])

        self.y1 = tf.nn.relu(tf.matmul(self.x, self.W_1) + self.b_1)
        self.y = tf.nn.sigmoid(tf.matmul(self.y1, self.W_2) + self.b_2)

        self.sess = tf.Session()
        self.sess.run(tf.global_variables_initializer())

    def __call__(self, x):
        return self.sess.run(self.y, feed_dict={self.x: x})

    def weight_variable(self, shape):
        initial = tf.truncated_normal(shape, stddev=0.1)
        return tf.Variable(initial)

    def bias_variable(self, shape):
        initial = tf.constant(0.1, shape=shape)
        return tf.Variable(initial)


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


