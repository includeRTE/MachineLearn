import numpy as np
from numpy import *
import tensorflow as tf

g1 = tf.Graph()
with g1.as_default():
    _initializer = tf.zeros_initializer()
    _initializer.shape = [1]
    v = tf.get_variable("v",initializer=_initializer)

g2 =  tf.Graph()
with g2.as_default():
    v = tf.get_variable("v",initializer=tf.zeros_initializer(shape=[1]))

with tf.Session(graph=g1) as sess:
    tf.initialize_all_variables().run()
    with tf.variable_scope("",ersuse=True):
        print(sess.run(tf.get_variable("v")))

with tf.Session(graph=g2) as sess:
    tf.initialize_all_variables().run()
    with tf.variable_scope("",ersuse=True):
        print(sess.run(tf.get_variable("v")))

g = tf.Graph()
with g.device(None):
    result = a + b