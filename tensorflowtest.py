import numpy as np
from numpy import *
import tensorflow as tf

g1 = tf.Graph()
with g1.as_default():
    v = tf.get_variable("v",shape=[1],initializer=tf.zeros_initializer())

g2 =  tf.Graph()
with g2.as_default():
    v = tf.get_variable("v",shape=[1],initializer=tf.ones_initializer())

with tf.Session(graph=g1) as sess:
    tf.initialize_all_variables().run()
    with tf.variable_scope("",reuse=True):
        print(sess.run(tf.get_variable("v")))

with tf.Session(graph=g2) as sess:
    tf.initialize_all_variables().run()
    with tf.variable_scope("",reuse=True):
        print(sess.run(tf.get_variable("v")))

w1 = tf.Variable(tf.random_normal([2,3],stddev=1,seed=1))
w2 = tf.Variable(tf.random_normal([3,1],stddev=1,seed=1))

x=tf.constant([[0.7,0.9]])
a = tf.matmul(x,w1)
y = tf.matmul(a,w2)

sess = tf.Session()
init_op = tf.initialize_all_variables()
print(sess.run(init_op))
sess.close()
class TensorFlowFirst():
    def __init__(self):
        self.sess = tf.Session()
        self.batch_size = 8
        self.w1 = tf.Variable(tf.random_normal([2,3],stddev=1,seed=1))
        self.w2 = tf.Variable(tf.random_normal([3,1],stddev=1,seed=1))
        self.x = tf.placeholder(tf.float32,shape=(None,2),name='x-input')
        self.y_ = tf.placeholder(tf.float32,shape=(None,1),name='y-input')
        self.a = tf.matmul(x,w1)
        self.y_ = tf.matmul(a,w2)
        self.cross_entropy = -tf.reduce_mean(y_ * tf.log(tf.clip_by_value(y,1e-10,1.0)))
        self.train_step = tf.train.AdamOptimizer(0.001).minimize(self.cross_entropy)
        self.rdm  = RandomState(1)
        self.data_size = 128
        
    
'''g = tf.Graph()
with g.device(None):
    result = a + b'''