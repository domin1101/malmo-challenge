#pragma once

#ifndef _TENSORFLOWEXPORTER_H_
#define _TENSORFLOWEXPORTER_H_

// Includes
#include "LightBulb/IO/Exporter/AbstractNetworkExporter.hpp"

/**
* \brief Exports a neural network for using in Tensorflow.
*/
class TensorflowExporter : public LightBulb::AbstractNetworkExporter
{
private:
public:
	// Inherited:
	std::string exportToString(const LightBulb::AbstractNeuralNetwork& neuralNetwork) override;
	std::string getName() const override;
	std::string getFileExtensions() const override;
};

#endif