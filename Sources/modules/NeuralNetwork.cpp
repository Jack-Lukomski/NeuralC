#include "../include/NeuralNetwork.hpp"

NeuralNetwork::NeuralNetwork(NeuralNetArch_t & architecture)
{
    _layerCount = architecture.size()-1;
    _activations.push_back(arma::mat(1, architecture[0], arma::fill::zeros));

    for (size_t i = 0; i < _layerCount; i++)
    {
        _weights.push_back(arma::mat(_activations[i].n_cols, architecture[i + 1], arma::fill::zeros));
        _biases.push_back(arma::mat(1, architecture[i + 1], arma::fill::zeros));
        _activations.push_back(arma::mat(1, architecture[i + 1], arma::fill::zeros));
    }
}

void NeuralNetwork::randomize(float min, float max)
{
    for (size_t i = 0; i < _layerCount; i++) {
        _weights[i] = (arma::randu<arma::mat>(_weights[i].n_rows, _weights[i].n_cols) * (max - min) + min);
        _biases[i] = (arma::randu<arma::mat>(_biases[i].n_rows, _biases[i].n_cols) * (max - min) + min);
    }
}

void NeuralNetwork::print() const 
{
    for (size_t i = 0; i < _layerCount; i++) {
        std::cout << _weights[i] << "\n" << _biases[i] << std::endl;
    }
}