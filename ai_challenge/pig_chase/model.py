
from __future__ import absolute_import

import itertools

import zignor
from malmopy.model import BaseModel
import tensorflow as tf

class MLPTensor:
    def __init__(self, in_shape, output_shape, hidden_layer_sizes, sess):
        self.input_shape = in_shape
        self.output_shape = output_shape

        with tf.name_scope('NN'):

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

        self.sess = sess

    def __call__(self, x):
       # run_options = tf.RunOptions(trace_level=tf.RunOptions.FULL_TRACE)
       # run_metadata = tf.RunMetadata()

        ret = self.sess.run(self.y[-1], feed_dict={self.x: x})#, options=run_options, run_metadata=run_metadata)

       # writer = tf.summary.FileWriter("test", self.sess.graph)
      #  writer.add_run_metadata(run_metadata, "Blub")
       # exit(0)

        return ret

    def weight_variable(self, shape):
        initial = tf.truncated_normal(shape, stddev=0.1)
        return tf.Variable(initial, name="weights")

    def bias_variable(self, shape):
        initial = tf.constant(0.1, shape=shape)
        return tf.Variable(initial, name="bias")

    def mutation_strength_variable(self, shape):
        initial = tf.constant(0.2, shape=shape)
        return tf.Variable(initial, name="mut_weights")

    def copyWeightsFrom(self, other):
        with tf.name_scope('copy'):
            for own_W, other_W, own_mut_W, other_mut_W in itertools.izip(self.W, other.W, self.mut_W, other.mut_W):
                self.sess.run(own_W.assign(other_W))
                self.sess.run(own_mut_W.assign(other_mut_W))

            for own_b, other_b, own_mut_b, other_mut_b in itertools.izip(self.b, other.b, self.mut_b, other.mut_b):
                self.sess.run(own_b.assign(other_b))
                self.sess.run(own_mut_b.assign(other_mut_b))

    def _mutate_tensor(self, target_W, W, target_mut_W, mut_W):
        with tf.name_scope('mut_W'):
            target_mut_W = mut_W * tf.exp(self.mutationStrengthChangeSpeed * zignor.randn(*mut_W.shape.as_list()))
        with tf.name_scope('normalize'):
            target_mut_W = tf.minimum(self.mutationStrengthMax, tf.maximum(self.mutationStrengthMin, target_mut_W))
        with tf.name_scope('W_add'):
            target_W = W + target_mut_W * zignor.randn(*target_mut_W.shape.as_list())

       # run_options = tf.RunOptions(trace_level=tf.RunOptions.FULL_TRACE)
        #run_metadata = tf.RunMetadata()

        self.sess.run(target_W)#, options=run_options, run_metadata=run_metadata)

        #writer = tf.summary.FileWriter("test", self.sess.graph)
        #writer.add_run_metadata(run_metadata, "Blub")
        #exit(0)


    def mutate_and_assign(self, other):
        with tf.name_scope('mutate'):
            with tf.name_scope('W'):
                for own_W, W, own_mut_W, mut_W in itertools.izip(self.W, other.W, self.mut_W, other.mut_W):
                    self._mutate_tensor(own_W, W, own_mut_W, mut_W)

            with tf.name_scope('b'):
                for own_b, b, own_mut_b, mut_b in itertools.izip(self.b, other.b, self.mut_b, other.mut_b):
                    self._mutate_tensor(own_b, b, own_mut_b, mut_b)

    def randomize(self):
        with tf.name_scope('randomize'):
            for W in self.W:
                W.assign(tf.truncated_normal(W.shape, stddev=0.1))

            for b in self.b:
                b.assign(tf.constant(0.1, shape=b.shape))

            for mut_b in self.mut_b:
                mut_b.assign(tf.constant(0.2, shape=mut_b.shape))

            for mut_W in self.mut_W:
                mut_W.assign(tf.constant(0.2, shape=mut_W.shape))

    def avg_mutation_strength(self):
        mut_W_sum_list = [tf.reduce_sum(mat) for mat in self.mut_W]
        mut_b_sum_list = [tf.reduce_sum(mat) for mat in self.mut_b]


        #avg = (tf.add_n(mut_W_sum_list) + tf.add_n(mut_b_sum_list)) /

        #tf.summary.FileWriter("test", self.sess.graph)
        #exit(0)

#import torch.nn as nn
#import torch.nn.functional as F

# class MLPTorch(nn.Module):
#
#     def __init__(self):
#         super(MLPTorch, self).__init__()
#         self.fc1 = nn.Linear(18 * 18, 1)
#         self.fc2 = nn.Linear(1, 3)
#
#     def forward(self, x):
#         x = F.relu(self.fc1(x))
#         x = F.sigmoid(self.fc2(x))
#         return x


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

    def mutate_and_assign(self, other):
        self._model.mutate_and_assign(other._model)

    def randomize(self):
        self._model.randomize()

    def avg_mutation_strength(self):
        return self._model.avg_mutation_strength()



