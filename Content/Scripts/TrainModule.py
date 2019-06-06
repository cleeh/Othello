import gc
import random
import math

import tensorflow as tf
import numpy as np
import unreal_engine as ue
from TFPluginAPI import TFPluginAPI

#==================== Constants ====================#
# about Game
NONE = 1
BLACK = 2
WHITE = 3

# about Hypothesis
GRIDS = 8
INPUTS = GRIDS * GRIDS
HIDDEN1S = 120
HIDDEN2S = 120
OUTPUTS = GRIDS * GRIDS

# about Learning
LEARNINGRATE = 0.01
DISCOUNT = 0.1
EPSILONMINVALUE = 0.001

# about ReplayMemory
MAXMEMORY = 64
#=================================================#

class ReplayMemory:
	def __init__(self):
		self.reset()
		pass

	def reset(self):
		self.CurrentState = np.empty((MAXMEMORY, INPUTS), dtype = np.float32)
		self.NextState = np.empty((MAXMEMORY, INPUTS), dtype = np.float32)
		self.Actions = np.zeros(MAXMEMORY, dtype = np.uint8)
		self.Rewards = np.empty(MAXMEMORY, dtype = np.int8)
		self.IsGameOver = np.empty(MAXMEMORY, dtype = np.bool)

		self.Count = 0
		pass

	def save(self, current_state, next_state, action, reward, game_over):
		self.CurrentState[self.Count, ...] = current_state
		self.NextState[self.Count, ...] = next_state
		self.Actions[self.Count] = action
		self.Rewards[self.Count] = reward
		self.IsGameOver[self.Count] = game_over

		self.Count += 1
		pass

	def getBatch(self, sess, model, input_placeholder):
		Inputs = np.zeros((self.Count, INPUTS))
		Targets = np.zeros((self.Count, OUTPUTS))

		for i in range(self.Count):
			CurrentState = np.reshape(self.CurrentState[i], (1, INPUTS))
			Target = sess.run(model, feed_dict={input_placeholder: CurrentState})

			NextState = np.reshape(self.NextState[i], (1, INPUTS))
			Output = sess.run(model, feed_dict={input_placeholder: NextState})

			# Gives max Q-value for the next state
			if(self.IsGameOver[i] == True):
				Target[0, [self.Actions[i]]] = self.Rewards[i]
			else:
				Target[0, [self.Actions[i]]] = self.Rewards[i] + DISCOUNT * np.amax(Output)

			# Update 'Inputs' & 'Targets'
			Inputs[i] = CurrentState
			Targets[i] = Target

		return Inputs, Targets

class ExampleAPI(TFPluginAPI):
	def onSetup(self):
		#==================== Initialize ====================#
		# State
		self.reset()

		#==================== Hypothesis ====================#
		self.input = tf.placeholder(tf.float32, shape=[None, INPUTS], name='input')

		w1 = tf.Variable(tf.truncated_normal(shape=[INPUTS, HIDDEN1S]), dtype=tf.float32, name='w1')
		b1 = tf.Variable(tf.zeros(shape=[HIDDEN1S]), dtype=tf.float32, name='b1')
		hidden1 = tf.sigmoid(tf.matmul(self.input, w1) + b1, name='hidden1')

		w2 = tf.Variable(tf.truncated_normal(shape=[HIDDEN1S, HIDDEN2S]), dtype=tf.float32, name='w2')
		b2 = tf.Variable(tf.zeros(shape=[HIDDEN2S]), dtype=tf.float32, name='b2')
		hidden2 = tf.sigmoid(tf.matmul(hidden1, w2) + b2, name='hidden2')

		wo = tf.Variable(tf.truncated_normal(shape=[HIDDEN2S, OUTPUTS]), dtype=tf.float32, name='wo')
		bo = tf.Variable(tf.zeros(shape=[OUTPUTS]), dtype=tf.float32, name='bo')
		self.output = tf.sigmoid(tf.matmul(hidden2, wo) + bo, name='output')

		self.target = tf.placeholder(tf.float32, shape=[None, OUTPUTS], name='target')

		#==================== Final Process ====================#
		# Cost & Optimizer
		self.cost = tf.reduce_mean(tf.square(self.output - self.target), 1, name='cost')
		self.optimizer = tf.train.GradientDescentOptimizer(LEARNINGRATE, name='optimizer').minimize(self.cost, name='minimizer')

		# File Path
		self.scripts_path = ue.get_content_dir() + "Scripts"
		self.model_directory = self.scripts_path + "/model"
		self.model_path = self.model_directory + "/model.ckpt"		

		# Session & Saver
		self.sess = tf.Session()
		self.saver = tf.train.Saver()
		try:
			self.saver.restore(self.sess, tf.train.latest_checkpoint(self.model_directory))
			ue.log('loaded model')
		except:
			self.sess.run(tf.global_variables_initializer())
			ue.log('no stored model')

		# ReplayMemory
		self.Memory = ReplayMemory()
		self.LastAction = -1

		# Epsilon
		self.Epsilon = 1

		# Game
		self.Sequence = 0
		self.PlayNumber = 1
		pass

	def onJsonInput(self, jsonInput):
		#==================== Initialize ====================#
		NewState = np.reshape([jsonInput["state"]], (-1, INPUTS))
		Reward = jsonInput["reward"]
		IsGameOver = jsonInput["gameover"]
		Putable = jsonInput["putable"]

		action = -1
		if(IsGameOver):
			action = self.LastAction
		else:
			PutableList = []
			for i in range(INPUTS):
				if(Putable[i]):
					PutableList.append(i)
			if(len(PutableList) <= 0):
				return -1

			self.Sequence += 1
			ue.log('==================================== ' + str(self.Sequence) + ' ====================================')

			selector = random.random()
			if (selector <= self.Epsilon):
				index = random.randrange(0, len(PutableList))
				if(index > 0):
					index -= 1
				action = PutableList[index]
				ue.log('#Random: ' + str(action))
				ue.log(action)
			else:
				q = self.sess.run(self.output, feed_dict={self.input: NewState})
				action = q.argmax()
				ue.log('#DQN: ' + str(action))
				for i in range(len(PutableList)):
					if(action == PutableList[i]):
						break
				index = random.randrange(0, len(PutableList))
				action = PutableList[index]
				ue.log('-> Random: (' + str(action) + ')')

			# Decay the epsilon by multiplying by 0.999, not allowing it to go below a certain threshold.
			if(self.Epsilon > EPSILONMINVALUE):
				self.Epsilon *= 0.9999

		self.Memory.save(self.State, NewState, action, Reward, IsGameOver)
		self.LastAction = action
		self.State = NewState

		return int(action)

	def onBeginTraining(self):
		# We get a batch of training data to train the model.
		Inputs, Targets = self.Memory.getBatch(self.sess, self.output, self.input)

		_, loss = self.sess.run([self.optimizer, self.cost], feed_dict={self.input: Inputs, self.target: Targets})
		ue.log('==================================== Targets, loss ====================================')
		ue.log(Targets)
		ue.log(loss)
		ue.log('============================================================================')

		if(self.Sequence % 100 == 0):
			save_path = self.saver.save(self.sess, self.model_path)#, global_step=training_range)
			ue.log("saved model")

		self.reset()
		self.Memory.reset()
		self.PlayNumber += 1
		ue.log('#PlayNumber: ' + str(self.PlayNumber))

		return {}

	def onStopTraining(self):
		pass

	def reset(self):
		self.State = np.zeros((1, INPUTS))
		self.State[0][27] = BLACK
		self.State[0][36] = BLACK
		self.State[0][28] = WHITE
		self.State[0][35] = WHITE
		pass

def getApi():
	#return CLASSNAME.getInstance()
	return ExampleAPI.getInstance()