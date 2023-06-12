#include "../include/NeuralNetwork.hpp"

NeuralNetwork::NeuralNetwork(uint32_t numInputs, 
                             uint32_t numHiddenLayers, 
                             const std::vector<uint32_t>& numHiddenNeurons, 
                             uint32_t numOutputs) 
                            : numInputs_(numInputs), 
                            numHiddenLayers_(numHiddenLayers), 
                            numHiddenNeurons_(numHiddenNeurons), 
                            numOutputs_(numOutputs) 
{
    arma::mat inputLayer_mat = arma::zeros<arma::mat>(1, numInputs);
    Layer inputLayer(inputLayer_mat, LayerType::InputLayer);
    layers_.push_back(inputLayer);

    uint32_t prevLayerNeuronCt = numInputs;
    for (uint32_t layerIndex = 0; layerIndex < numHiddenLayers; layerIndex++)
    {
        arma::mat hiddenLayerWeights_mat = arma::zeros<arma::mat>(prevLayerNeuronCt, numHiddenNeurons[layerIndex]);
        arma::mat hiddenLayerBiases_mat = arma::zeros<arma::mat>(1, numHiddenNeurons[layerIndex]);
        Layer hiddenLayer(hiddenLayerWeights_mat, hiddenLayerBiases_mat, LayerType::HiddenLayer);
        layers_.push_back(hiddenLayer);
        prevLayerNeuronCt = numHiddenNeurons[layerIndex];
    }

    arma::mat outputLayerWeights_mat = arma::zeros<arma::mat>(prevLayerNeuronCt, numOutputs);
    arma::mat outputLayerBiases_mat = arma::zeros<arma::mat>(1, numOutputs);
    Layer outputLayer(outputLayerWeights_mat, outputLayerBiases_mat, LayerType::OutputLayer);
    layers_.push_back(outputLayer);
}

std::vector<arma::mat> NeuralNetwork::forwardProp(arma::mat & input, arma::mat (*activationFunction)(const arma::mat&))
{
    std::vector<arma::mat> outputs;

    layers_[0].setLayerWeights(input);
    arma::mat layerOutput = layers_[0].getLayerWeights();
    outputs.push_back(layerOutput);

    for (uint32_t layerIndex = 1; layerIndex < layers_.size(); layerIndex++)
    {
        layerOutput = layerOutput * layers_[layerIndex].getLayerWeights();
        layerOutput = layerOutput + layers_[layerIndex].getLayerBiases();
        layerOutput = activationFunction(layerOutput);
        outputs.push_back(layerOutput);
    }
    
    return outputs;
}

void NeuralNetwork::train(arma::mat & inputs, 
            arma::mat & target, 
            double learningRate, 
            uint32_t epochs, 
            arma::mat (*activationFunction)(const arma::mat&), 
            arma::mat (*derivativeFunction)(const arma::mat&))
{
    for (uint32_t epoch = 0; epoch < epochs; epoch++)
    {
        std::vector<arma::mat> outputs = NeuralNetwork::forwardProp(inputs, activationFunction);
        arma::mat output = outputs.back();

        arma::mat outputError = target - output;

        for (uint32_t layerIndex = layers_.size() - 1; layerIndex >= 1; layerIndex--)
        {
            Layer & currLayer = layers_[layerIndex];
            arma::mat gradients = derivativeFunction(outputs[layerIndex]) % outputError * learningRate;
            arma::mat deltas = gradients * outputs[layerIndex-1].t();

            currLayer.setLayerWeights(currLayer.getLayerWeights() + deltas);
            currLayer.setLayerBiases(currLayer.getLayerBiases() + gradients);

            outputError = currLayer.getLayerWeights().t() * outputError;
        }
    }
}

void NeuralNetwork::randomize()
{
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);

    for (auto & layer: layers_)
    {
        if (layer.getLayerType() != LayerType::InputLayer)
        {
            arma::mat randWeights_mat(layer.getLayerWeights().n_rows, layer.getLayerWeights().n_cols);
            arma::mat randBiases_mat(layer.getLayerBiases().n_rows, layer.getLayerBiases().n_cols);
            
            randWeights_mat.imbue([&]() { return distribution(generator); });
            randBiases_mat.imbue([&]() { return distribution(generator); });

            layer.setLayerWeights(randWeights_mat);
            layer.setLayerBiases(randBiases_mat);
        }
    }
}

void NeuralNetwork::printNetwork() const 
{
    for (const auto & layer: layers_)
    {
        layer.printLayer();
    }
}

NeuralNetwork::~NeuralNetwork() {}