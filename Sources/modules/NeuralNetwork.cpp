#include "../include/NeuralNetwork.hpp"

NeuralNetwork::NeuralNetwork(NeuralNetArch_t & architecture, ActivationType af)
{
    _arch = architecture;
    _af = af;
    _layerCount = architecture.size()-1;
    _activations.push_back(arma::mat(1, architecture[0], arma::fill::zeros));

    for (size_t i = 0; i < _layerCount; ++i) {
        _weights.push_back(arma::mat(_activations[i].n_cols, architecture[i + 1], arma::fill::zeros));
        _biases.push_back(arma::mat(1, architecture[i + 1], arma::fill::zeros));
        _activations.push_back(arma::mat(1, architecture[i + 1], arma::fill::zeros));
    }
}

void NeuralNetwork::forwardProp()
{
    A_Func actFunc(_af);
    for (size_t i = 0; i < _layerCount; ++i) {
        _activations[i + 1] = _activations[i] * _weights[i];
        _activations[i + 1] += _biases[i];
        actFunc.apply(_activations[i + 1]);
    }
}

void NeuralNetwork::learn(NeuralNetwork gradient, float learnRate)
{
    for (size_t i = 0; i < _layerCount; ++i) {
        for (size_t j = 0; j < _weights[i].n_rows; ++j) {
            for (size_t k = 0; k < _weights[i].n_cols; ++k) {
                _weights[i](j, k) -= learnRate * gradient._weights[i](j, k);
            }
        }
    }

    for (size_t i = 0; i < _layerCount; ++i) {
        for (size_t j = 0; j < _biases[i].n_rows; ++j) {
            for (size_t k = 0; k < _biases[i].n_cols; ++k) {
                _biases[i](j, k) -= learnRate * gradient._biases[i](j, k);
            }
        }
    }
}

NeuralNetwork NeuralNetwork::getGradientFiniteDif(const std::vector<arma::mat> & inputs, const std::vector<arma::mat> & outputs, float eps)
{
    NeuralNetwork gradient(_arch, _af);
    float cost = NeuralNetwork::getCost(inputs, outputs);
    float saved;

    for (size_t i = 0; i < _layerCount; ++i) {
        for (size_t j = 0; j < _weights[i].n_rows; ++j) {
            for (size_t k = 0; k < _weights[i].n_cols; ++k) {
                saved = _weights[i](j, k);
                _weights[i](j, k) += eps;
                gradient._weights[i](j, k) = ((NeuralNetwork::getCost(inputs, outputs) - cost) / eps);
                _weights[i](j, k) = saved;
            }
        }
    }

    for (size_t i = 0; i < _layerCount; ++i) {
        for (size_t j = 0; j < _biases[i].n_rows; ++j) {
            for (size_t k = 0; k < _biases[i].n_cols; ++k) {
                saved = _biases[i](j, k);
                _biases[i](j, k) += eps;
                gradient._biases[i](j, k) = ((NeuralNetwork::getCost(inputs, outputs) - cost) / eps);
                _biases[i](j, k) = saved;
            }
        }
    }

    return gradient;
}

float NeuralNetwork::getCost(const std::vector<arma::mat> & inputs, const std::vector<arma::mat> & outputs)
{
    assert(inputs.size() == outputs.size());

    float cost = 0.0;

    for (size_t i = 0; i < inputs.size(); ++i) {
        NeuralNetwork::setInput(inputs[i]);
        NeuralNetwork::forwardProp();
        arma::mat difference = NeuralNetwork::getOutput() - outputs[i];
        difference = difference % difference;
        cost += arma::accu(difference);
    }
    return cost/inputs.size();
}

void NeuralNetwork::setInput(const arma::mat & input)
{
    assert(input.n_rows == _activations[0].n_rows);
    assert(input.n_cols == _activations[0].n_cols);

    _activations[0] = input;
}

arma::mat NeuralNetwork::getOutput()
{
    return _activations[_layerCount];
}

void NeuralNetwork::randomize(float min, float max)
{
    for (size_t i = 0; i < _layerCount; ++i) {
        _weights[i] = (arma::randu<arma::mat>(_weights[i].n_rows, _weights[i].n_cols) * (max - min) + min);
        _biases[i] = (arma::randu<arma::mat>(_biases[i].n_rows, _biases[i].n_cols) * (max - min) + min);
    }
}

void NeuralNetwork::print() const 
{
    for (size_t i = 0; i < _layerCount; ++i) {
        std::cout << _weights[i] << "\n" << _biases[i] << std::endl;
    }
}