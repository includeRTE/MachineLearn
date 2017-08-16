import numpy as numpylib
import os      
import operator
import matplotlib
import matplotlib.pyplot as plt
from plistlib import PlistFormat
import pprint
from numpy import log
import math
import feedparser
from functools import reduce

def loadDataSet():
    postingList = [['my','dog','has','flea','problems','help','please'],
    ['maybe','not','take','him','to','dog','park','stupid'],
    ['my','dalmation','is','so','cute','I','love','him'],
    ['stop','posting','stupid','worthless','garbage'],
    ['mr','licks','ate','my','steak','how','to','stop','him'],
    ['quit','buying','worthless','dog','food','stupid']]
    classVec = [0,1,0,1,0,1]
    return postingList,classVec

def createVocabList(dataSet):
    vocabSet = set([])
    for document in dataSet:
        vocabSet = vocabSet | set(document)
    return list(vocabSet)

def setOfWords2Vec(vocabList,inputSet):
    returnVec = [0] * len(vocabList)
    for word in inputSet:
        if word in vocabList:
           returnVec[vocabList.index(word)] = 1
        else:
           print('the word:%s is not in my Vocabulary' % word)
    return returnVec

def trainNB0(trainMatrix,trainCategeory):
    numTrainDocs = len(trainMatrix)
    numWords = len(trainMatrix[0])
    pAbusive = sum(trainCategeory) / float(numTrainDocs)
    p0Num = numpylib.zeros(numWords)
    p1Num = numpylib.zeros(numWords)
    p0Denom = 0.0
    p1Denom = 0.0
    for i in range(numTrainDocs):
        if trainCategeory[i] == 1:
           p1Num += trainMatrix[i]
           p1Denom += sum(trainMatrix[i])
        else:
           p0Num += trainMatrix[i]
           p0Denom += sum(trainMatrix[i])
    p1Vect = p1Num / p1Denom
    p0Vect = (p0Num / p0Denom)
    return p0Vect,p1Vect,pAbusive

def classifyNB(vec2Classify,p0Vec,p1Vec,pClass1):
    p1 = reduce(lambda x,y:x * y,vec2Classify * p1Vec) * pClass1
    p0 = reduce(lambda x,y:x * y,vec2Classify * p0Vec) * (1 - pClass1)
    if p1 > p0:
       return 1
    else:
       return 0

def testingNB():
    listOPosts,listClasses = loadDataSet()
    myVocabList = createVocabList(listOPosts)
    trainMat = []
    for postinDoc in listOPosts:
        trainMat.append(setOfWords2Vec(myVocabList,postinDoc))
    p0V,p1V,pAb = trainNB0(numpylib.array(trainMat),numpylib.array(listClasses))
    testEntry = ['love','my','dalmation']
    thisDoc = numpylib.array(setOfWords2Vec(myVocabList,testEntry))
    print(testEntry,'classified as:',classifyNB(thisDoc,p0V,p1V,pAb))
    testEntry = ['stupid','garbage']
    thisDoc = numpylib.array(setOfWords2Vec(myVocabList,testEntry))
    print(testEntry,'classified as:',classifyNB(thisDoc,p0V,p1V,pAb))

def bagOfWords2VecMN(vocabList,inpuSet):
    returnVec = [0] * len(vocabList)
    for word in vocabList:
        if word in vocabList:
           returnVec[vocabList.index(word)] += 1
    return returnVec

def textParse(bigString):
    import re
    listOfTokens = re.split(r'\W*',bigString)
    return [tok.lower() for tok in listOfTokens if len(tok) > 2]
def spamTest():
    docList = [];classList = [];fullText = []
    for i in range(1,26):
        wordList = textParse(open('email/spam/%d.txt' % i).read())
        docList.append(wordList)
        fullText.extend(wordList)
        classList.append(1)
        wordList = textParse(open('email/ham/%d.txt' % i).read())
        docList.append(wordList)
        fullText.extend(wordList)
        classList.append(0)
    vocabList = createVocabList(docList)
    trainingSet = list(range(50));testSet=[]
    for i in range(10):
        randIndex = int(numpylib.random.uniform(0,len(trainingSet)))
        testSet.append(trainingSet[randIndex])
        del(trainingSet[randIndex])
    trainMat = [];trainClasses = []
    for docIndex in trainingSet:
        trainMat.append(setOfWords2Vec(vocabList,docList[docIndex]))
        trainClasses.append(classList[docIndex])
    p0V,p1V,pSpam = trainNB0(numpylib.array(trainMat),numpylib.array(trainClasses))
    errorCount = 0
    for docIndex in testSet:
        wordVector = setOfWords2Vec(vocabList,docList[docIndex])
        if classifyNB(numpylib.array(wordVector),p0V,p1V,pSpam) != classList[docIndex]:
           errorCount += 1
    print('the error rate is:',float(errorCount)/len(testSet))

def calcMostFreq(vocabList,fullText):
    import operator
    freqDict = {}
    for token in vocabList:
        freqDict[token] = fullText.count(token)
    sortedFreq = sorted(freqDict.items(),key=operator.itemgetter(1),reverse=True)
    return sortedFreq[:40]
def localWords(feed1,feed0):
    import feedparser
    docList = [];classList = [];fullText = []
    minLen = min(len(feed1['entries']),len(feed0['entries']))
    for i in range(minLen):
        wordList = textParse(feed1['entries'][i]['summary'])
        docList.append(wordList)
        fullText.extend(wordList)
        classList.append(1)
        wordList = textParse(feed0['entries'][i]['summary'])
        docList.append(wordList)
        fullText.extend(wordList)
        classList.append(0)
    vocabList = createVocabList(docList)
    top30Words = calcMostFreq(vocabList,fullText)
    for parW in top30Words:
        if parW[0] in vocabList:
           vocabList.remove(parW[0])
    trainingSet = list(range(2*minLen));testSet=[]
    for i in range(20):
        randIndex = int(numpylib.random.uniform(0,len(trainingSet))) 
        testSet.append(trainingSet[randIndex])
        del(trainingSet[randIndex])
    trainMat = []; trainClasses = []
    for docIndex in  trainingSet:
        trainMat.append(bagOfWords2VecMN(vocabList,docList[docIndex]))
        trainClasses.append(classList[docIndex])
    p0V,p1V,pSpam = trainNB0(numpylib.array(trainMat),numpylib.array(trainClasses))
    errorCount = 0
    for docIndex in testSet:
        wordVector = bagOfWords2VecMN(vocabList,docList[docIndex])
        if classifyNB(numpylib.array(wordVector),p0V,p1V,pSpam) != classList[docIndex]:
           errorCount += 1
    print('the error rate is',float(errorCount)/len(testSet))
    return vocabList,p0V,p1V
if __name__ == '__main__':
    '''listOPosts,listClasses = loadDataSet()
    myVocabList = createVocabList(listOPosts)
    trainMat = []
    for postinDoc in listOPosts:
        trainMat.append(setOfWords2Vec(myVocabList,postinDoc))
    p0V,p1V,pAb = trainNB0(trainMat,listClasses)
    print(pAb)
    print(p0V)'''
    #testingNB()
    #spamTest()
    ny = feedparser.parse('http://newyork.craigslist.org/stp/index.rss')
    sf = feedparser.parse('http://sfbay.craigslist.org/stp/index.rss')
    vocabList,pSF,pNY = localWords(ny,sf)
