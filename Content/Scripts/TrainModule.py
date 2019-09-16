import gc
import random
import math

import tensorflow as tf
import numpy as np
import unreal_engine as ue
from TFPluginAPI import TFPluginAPI

#==================== Constants ====================#
# about Game
NONE = 0
BLACK = 1
WHITE = 2

# about Hypothesis
GRIDS = 8
INPUTS = GRIDS * GRIDS
HIDDEN1S = INPUTS * 2
HIDDEN2S = HIDDEN1S
OUTPUTS = GRIDS * GRIDS

# about Learning
LEARNINGRATE = 1e-45
DISCOUNT = 0.95
EPSILONMINVALUE = 0.01

# about ReplayMemory
MAXMEMORY = INPUTS * 2
MAXBATCHSIZE = INPUTS
#=================================================#





class ReplayMemory:
	def __init__(self):
		self.CurrentState = np.empty((MAXMEMORY, INPUTS), dtype = np.float32)
		self.NextState = np.empty((MAXMEMORY, INPUTS), dtype = np.float32)
		self.Actions = np.zeros(MAXMEMORY, dtype = np.uint8)
		self.Rewards = np.empty(MAXMEMORY, dtype = np.uint8)
		self.IsGameOver = np.empty(MAXMEMORY, dtype = np.bool)

		self.Count = 0
		self.Pointer = 0
		pass

	def save(self, current_state, next_state, action, reward, game_over):
		self.CurrentState[self.Pointer, ...] = current_state
		self.NextState[self.Pointer, ...] = next_state
		self.Actions[self.Pointer] = action
		self.Rewards[self.Pointer] = reward
		self.IsGameOver[self.Pointer] = game_over

		self.Count = max(self.Count, self.Pointer + 1)
		self.Pointer = (self.Pointer + 1) % MAXMEMORY
		pass

	def getBatch(self, sess, model_t, input_placeholder):
		BatchSize = min(self.Count, MAXBATCHSIZE)
		Inputs = np.zeros((BatchSize, INPUTS))
		Targets = np.zeros((BatchSize, OUTPUTS))

		Length = self.Count
		for i in range(BatchSize):
			if Length == 1:
				Length = 2

			# Choose a random memory experience
			RandomIndex = random.randrange(1, Length)

			CurrentState = np.reshape(self.CurrentState[RandomIndex], (1, INPUTS))
			Target = sess.run(model_t, feed_dict={input_placeholder: CurrentState})

			NextState = np.reshape(self.NextState[RandomIndex], (1, INPUTS))
			Output = sess.run(model_t, feed_dict={input_placeholder: NextState})

			# Gives max Q-value for the next state
			if(self.IsGameOver[RandomIndex] == True):
				Target[0, [self.Actions[RandomIndex]]] = self.Rewards[RandomIndex]
			else:
				Target[0, [self.Actions[RandomIndex]]] = self.Rewards[RandomIndex] + DISCOUNT * np.amax(Output)

			# Update 'Inputs' & 'Targets'
			Inputs[i] = CurrentState
			Targets[i] = Target
		return Inputs, Targets





class ExampleAPI(TFPluginAPI):
	def onSetup(self):
		#==================== Initialize ====================#
		# File Path
		self.scripts_path = ue.get_content_dir() + "Scripts"
		self.model_directory = self.scripts_path + "/model"
		self.model_path = self.model_directory + "/model.ckpt"	

		# Game
		self.Sequence = 1
		self.PlayNumber = 1

		# Epsilon
		self.Epsilon = EPSILONMINVALUE

		# ReplayMemory
		self.Memory = ReplayMemory()
		self.LastAction = -1

		# State
		self.reset()

		#==================== Hypothesis ====================#
		self.input = tf.placeholder(tf.float32, shape=[None, INPUTS])

		# Model
		w1 = tf.Variable(tf.truncated_normal(shape=[INPUTS, HIDDEN1S], stddev=1.0 / math.sqrt(float(INPUTS))), dtype=tf.float32, name='w1')
		b1 = tf.Variable(tf.truncated_normal(shape=[HIDDEN1S], stddev=0.01), dtype=tf.float32, name='b1')
		hidden1 = tf.nn.relu(tf.matmul(self.input, w1) + b1, name='hidden1')

		w2 = tf.Variable(tf.truncated_normal(shape=[HIDDEN1S, HIDDEN2S], stddev=1.0 / math.sqrt(float(HIDDEN1S))), dtype=tf.float32, name='w2')
		b2 = tf.Variable(tf.truncated_normal(shape=[HIDDEN2S], stddev=0.01), dtype=tf.float32, name='b2')
		hidden2 = tf.nn.relu(tf.matmul(hidden1, w2) + b2, name='hidden2')

		wo = tf.Variable(tf.truncated_normal(shape=[HIDDEN2S, OUTPUTS], stddev=1.0 / math.sqrt(float(HIDDEN2S))), dtype=tf.float32, name='wo')
		bo = tf.Variable(tf.truncated_normal(shape=[OUTPUTS], stddev=0.01), dtype=tf.float32, name='bo')
		self.output = tf.matmul(hidden2, wo) + bo

		# Target
		w1_t = tf.Variable(tf.truncated_normal(shape=[INPUTS, HIDDEN1S], stddev=1.0 / math.sqrt(float(INPUTS))), dtype=tf.float32, name='w1_t')
		b1_t = tf.Variable(tf.truncated_normal(shape=[HIDDEN1S], stddev=0.01), dtype=tf.float32, name='b1_t')
		hidden1_t = tf.nn.relu(tf.matmul(self.input, w1_t) + b1_t, name='hidden1')

		w2_t = tf.Variable(tf.truncated_normal(shape=[HIDDEN1S, HIDDEN2S], stddev=1.0 / math.sqrt(float(HIDDEN1S))), dtype=tf.float32, name='w2_t')
		b2_t = tf.Variable(tf.truncated_normal(shape=[HIDDEN2S], stddev=0.01), dtype=tf.float32, name='b2_t')
		hidden2_t = tf.nn.relu(tf.matmul(hidden1_t, w2_t) + b2_t, name='hidden2')

		wo_t = tf.Variable(tf.truncated_normal(shape=[HIDDEN2S, OUTPUTS], stddev=1.0 / math.sqrt(float(HIDDEN2S))), dtype=tf.float32, name='wo_t')
		bo_t = tf.Variable(tf.truncated_normal(shape=[OUTPUTS], stddev=0.01), dtype=tf.float32, name='bo_t')
		self.output_t = tf.matmul(hidden2_t, wo_t) + bo_t

		# Cost & Optimizer
		self.target = tf.placeholder(tf.float32, shape=[None, OUTPUTS])
		self.cost = tf.reduce_mean(tf.square(self.output - self.target)) / 2
		self.optimizer = tf.train.AdamOptimizer(LEARNINGRATE).minimize(self.cost)

		#==================== Session & Saver ====================#
		self.sess = tf.Session()
		self.saver = tf.train.Saver()
		ue.log('######################################################')
		try:
			self.saver.restore(self.sess, self.model_path)
			ue.log('#################### loaded model ####################')
		except:
			self.sess.run(tf.global_variables_initializer())
			ue.log('################## no stored model ##################')
		ue.log('######################################################')
		pass

	def onJsonInput(self, jsonInput):
		#==================== Initialize ====================#
		NewState = np.reshape([jsonInput["state"]], (-1, INPUTS))
		Reward = jsonInput["reward"]
		IsGameOver = jsonInput["gameover"]
		Putable = jsonInput["putable"]

		action = -1
		PutableList = []
		for i in range(INPUTS):
			if(Putable[i]):
				PutableList.append(i)
		if(len(PutableList) <= 0):
			return -1
		#===============================================#
		
		ue.log('==================================== ' + str(self.Sequence) + ' ====================================')
		self.Sequence += 1

		selector = random.random()
		if (selector <= self.Epsilon):
			index = random.randrange(0, len(PutableList))
			action = PutableList[index]
			ue.log('#Random: ' + str(action))
		else:
			q = self.sess.run(self.output, feed_dict={self.input: NewState})
			action = q.argmax()

			flag = 1
			ue.log('#DQN: ' + str(action))
			for i in range(len(PutableList)):
				if(action == PutableList[i]):
					flag = 0
					break
			if(flag):
				self.giveDifference(NewState, action, q, -10.0)
				index = random.randrange(0, len(PutableList))
				action = PutableList[index]
				ue.log('-> Random: (' + str(action) + ')')
			ue.log('==================================== Actions ====================================')
			ue.log(q)

		# Decay the epsilon by multiplying by 0.999, not allowing it to go below a certain threshold.
		if(self.Epsilon > EPSILONMINVALUE):
			self.Epsilon *= 0.999

		# We get a batch of training data to train the model.
		Inputs, Targets = self.Memory.getBatch(self.sess, self.output_t, self.input)

		_, loss = self.sess.run([self.optimizer, self.cost], feed_dict={self.input: Inputs, self.target: Targets})
		ue.log('==================================== loss ====================================')
		ue.log(loss)

		self.Memory.save(self.State, NewState, action, Reward, IsGameOver)
		self.LastAction = action
		self.State = NewState

		return int(action)

	def onBeginTraining(self):
		if(self.PlayNumber % 100 == 0):
			self.saver.save(self.sess, self.model_path)
			ue.log('############################## saved model ##############################')
		ue.log('############################## PlayNumber: ' + str(self.PlayNumber) + '##############################')

		self.PlayNumber += 1
		self.reset()
		return {}

	def reset(self):
		self.State = np.zeros((1, INPUTS))
		self.State[0][27] = BLACK
		self.State[0][36] = BLACK
		self.State[0][28] = WHITE
		self.State[0][35] = WHITE
		pass

	def giveDifference(self, state, action, output, correction):
		Input = np.reshape(state, (1, INPUTS))
		Target = output.copy()
		Target[0, [action]] += correction
		self.sess.run(self.optimizer, feed_dict={self.input: Input, self.target: Target})
		pass





def getApi():
	#return CLASSNAME.getInstance()
	return ExampleAPI.getInstance()