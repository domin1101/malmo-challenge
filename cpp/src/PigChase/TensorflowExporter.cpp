// Includes
#include "TensorflowExporter.hpp"
#include "LightBulb/NeuralNetwork/NeuralNetwork.hpp"
#include "LightBulb/NetworkTopology/FeedForwardNetworkTopology.hpp"
#include "LightBulb/IO/Exporter/JSONObject.hpp"
#include "LightBulb/IO/Exporter/JSONArray.hpp"
#include "LightBulb/IO/Exporter/JSONNumberElement.hpp"
#include <fstream>

using namespace LightBulb;

std::string TensorflowExporter::exportToString(const AbstractNeuralNetwork& neuralNetwork)
{
	JSONArray layers;
	FeedForwardNetworkTopology& networkTopology = dynamic_cast<FeedForwardNetworkTopology&>(neuralNetwork.getNetworkTopology());

	int layerCount = networkTopology.getLayerCount();
	for (int l = 0; l < layerCount - 1; l++)
	{
		JSONObject* layer = new JSONObject();
		JSONArray* weights = new JSONArray();
		JSONArray* bias = new JSONArray();

		for (int r = 0; r < networkTopology.getAllWeights()[l].rows(); r++)
		{
			JSONArray* weightsPerNeuron = new JSONArray();
			for (int c = 0; c < networkTopology.getAllWeights()[l].cols(); c++)
			{
				if (networkTopology.usesBiasNeuron() && c == 0)
					bias->addElement(new JSONNumberElement<double>(networkTopology.getAllWeights()[l](r, c)));
				else
					weightsPerNeuron->addElement(new JSONNumberElement<double>(networkTopology.getAllWeights()[l](r, c)));
			}
			weights->addElement(weightsPerNeuron);
		}
		layer->addAttribute(new JSONAttribute("weights", weights));
		if (networkTopology.usesBiasNeuron())
			layer->addAttribute(new JSONAttribute("bias", bias));

		layers.addElement(layer);
	}

	return layers.toString();
}

std::string TensorflowExporter::getFileExtensions() const
{
	return "Tensorflow network (*.json)|*.json";
}

std::string TensorflowExporter::getName() const
{
	return "Tensorflow";
}