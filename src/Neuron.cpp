/*
 * Neuron.cpp
 *
 *  Created on: Sep 20, 2013
 *      Author: cheryuri
 */

#include "NeuralNetworks/Neuron.h"

/**
 * 		A neuron constructor.
 * 		Description: 	Constructor for neuron, based on a std::vector of already exist neurons and net function.
 * 		Purpose:	 	Creating a new neuron with neural links to every neuron in incoming vector and predefined function.
 * 		Prerequisites: 	Existence of a vector with neurons and net function.
*/

template <typename T>
Neuron<T>::Neuron(std::vector<Neuron< T> * >& inNeuronsLinkTo, NetworkFunction * inNetFunc)
{
	/*
	 * 		Net Function is an activation function for neuron
	*/


	mNetFunc = inNetFunc;

	/*
	 * 		Initially there is no input data, so sum of charges equal 0
	*/


	mSumOfCharges = 0.0;


	for(unsigned int i = 0; i < inNeuronsLinkTo.size(); i++){

		/*
		 *		Creating a link, based on Neuron from vector for every neuron in vector
		*/

		NeuralLink<T> * pLink = new NeuralLink<T>( inNeuronsLinkTo[i], 0.0 );

		/*
		 *		Newly created neuron will know who is linked to it, by maintaining a vector of links called mLinksToNeurons.
		*/

		mLinksToNeurons.push_back( pLink );

		/*
		 * 		A neuron, which is linked to newly created neuron, will know as well what its linked to, by maintaining a vector of input links.
		*/

		inNeuronsLinkTo[i]->SetInputLink( pLink );
/*		std::cin.get();
		NeuralLink * pInLink = inNeuronsLinkTo[i]->GetInputLink().back();
		pInLink->SetWeightCorrectionTerm(10);
		std::cout << pLink->GetWeightCorrectionTerm() << std::endl;
		std::cout << pLink << std::endl;
		std::cout << pInLink<< std::endl;
		std::cin.get();*/
	}

}

template <typename T>
Neuron<T>::~Neuron()
{
	//delete mNetFunc;

/*	for( unsigned int uNumOfLinks = 0; uNumOfLinks < mLinksToNeurons.size(); uNumOfLinks++){
		delete mLinksToNeurons[uNumOfLinks];
	}*/

}

template <typename T>
OutputLayerNeuronDecorator<T>::~OutputLayerNeuronDecorator()
{
	delete mNeuron;
}

template <typename T>
HiddenLayerNeuronDecorator<T>::~HiddenLayerNeuronDecorator()
{
	delete mNeuron;
}


template <typename T>
double HiddenLayerNeuronDecorator<T>::Fire()
{
	/*
	 * 		Hidden unit applies its activation function to compute its output signal
	 * 		and sends this signal to all units in the layer above (output units).
	*/

	for(int iLink = 0; iLink < this->GetNumOfLinks(); iLink++){

		NeuralLink<T> * pCurrentLink = mNeuron->at(iLink);
		Neuron<T> * pCurrentNeuronLinkedTo = pCurrentLink->GetNeuronLinkedTo();

		const double dWeight = mNeuron->at(iLink)->GetWeight();
		double	dCharge = mNeuron->GetSumOfCharges();
		double	dZj = (mNeuron->Process(dCharge));
		double 	dOutput = dZj*dWeight;

		pCurrentLink->SetLastTranslatedSignal(dZj);

		pCurrentNeuronLinkedTo->Input( dOutput );

		//std::cout << "Link: " << iLink << ", " << "Hidden Neuron fired: " << dOutput << " as func of: " << dCharge << " * " << dWeight << std::endl;
	}

	return mNeuron->GetSumOfCharges();
}

template <typename T>
double OutputLayerNeuronDecorator<T>::Fire()
{

	//double temp = mNeuron->GetSumOfCharges();
	double output = this->Process();
	mOutputCharge = output;
	//std::cout << "Output Neuron fired: " << output << " as func of: " << temp << std::endl;
	return output;

};

template <typename T>
double Neuron<T>::Fire()
{
	for(int iLink = 0; iLink < this->GetNumOfLinks(); iLink++){
		NeuralLink<T> * pCurrentLink = mLinksToNeurons[iLink];
		Neuron<T> * pCurrentNeuronLinkedTo = pCurrentLink->GetNeuronLinkedTo();

		const double dWeight = pCurrentLink->GetWeight();
		double	dCharge = mSumOfCharges;
		double 	dXi =  (mNetFunc->Process(dCharge));
		double 	dOutput = dXi*dWeight;

		pCurrentLink->SetLastTranslatedSignal(dXi);
		pCurrentNeuronLinkedTo->Input( dOutput );
		//std::cout << "Link: " << iLink << ", Neuron fired: " << dOutput << " as func of: " << dCharge << " * " << dWeight << std::endl;
		//mLinksToNeurons[iLink]->GetNeuronLinkedTo()->Input(mNetFunc->Process(mSumOfCharges*mLinksToNeurons[iLink]->GetWeight()));
	}
	//mSumOfCharges = 0;
	return mSumOfCharges;
}

template <typename T>
double	Neuron<T>::GetSumOfCharges()
{

	return mSumOfCharges;
}




template <typename T>
double Neuron<T>::PerformTrainingProcess(double inTarget)
{
	return 0;
}

template <typename T>
double HiddenLayerNeuronDecorator<T>::PerformTrainingProcess(double inTarget)
{

	/*
	 * 		Hidden unit sums its delta inputs from units in the layer above
	*/
	double dDeltaInputs = 0;
	for(int iOutputLink = 0; iOutputLink < (this->GetNumOfLinks()); iOutputLink++){
			NeuralLink<T> * pOutputLink = (this->GetLinksToNeurons()).at(iOutputLink);
			double dErrorInformationTerm = pOutputLink->GetErrorInFormationTerm();
			double dWeight = pOutputLink->GetWeight();
			dDeltaInputs = dDeltaInputs + (dWeight*dErrorInformationTerm);
	}

/*	for(int iOutputLink = 0; iOutputLink < (this->GetNumOfLinks()); iOutputLink++){
			NeuralLink * pOutputLink = (this->GetLinksToNeurons()).at(iOutputLink);
			pOutputLink->UpdateWeight();
	}*/

	double dErrorInformationTermj = dDeltaInputs * (this->Derivative());
	//std::cout << "dErrorInformationTermjHidden: " << dErrorInformationTermj << " as: " << dDeltaInputs << " * " << this->Derivative() << " .Derivative of:  " << mNeuron->GetSumOfCharges()<< std::endl;
	//std::cin.get();
	/*
	 * 		For every link to that hidden neuron, (inputLinks) calculate its weight correction term
	 * 		and update the link with it.
	*/
	for(unsigned int iInputLink = 0; iInputLink < (this->GetInputLink()).size(); iInputLink++){
		NeuralLink<T> * pInputLink = (this->GetInputLink()).at(iInputLink);
		double Xi = pInputLink->GetLastTranslatedSignal();
		double dWeightCorrectionTerm = Xi*dErrorInformationTermj;
		//std::cout << "dWeightCorrectionTerm: " << dWeightCorrectionTerm << std::endl;
		pInputLink->SetWeightCorrectionTerm(LearningRate*dWeightCorrectionTerm);


		/*
		 * 		Then hidden unit has to tell the input neurons the value of it ErrorInformationTerm, so we are setting its value
		 * 		in the link object.
		 */

		pInputLink->SetErrorInFormationTerm(dErrorInformationTermj);
	}
	return 0;
}

template <typename T>
double OutputLayerNeuronDecorator<T>::PerformTrainingProcess(double inTarget)
{
	double res;
	double dErrorInformationTerm = (inTarget - mOutputCharge) * mNeuron->Derivative();
	res = pow(inTarget - mOutputCharge,2);
	//std::cout << "dErrorInformationTermOutput: " << dErrorInformationTerm << " as: " << "(" << inTarget << " - " << mOutputCharge << ")" << " * " << mNeuron->Derivative() << " .Derivative of:  " << mNeuron->GetSumOfCharges()<< std::endl;
	//std::cin.get();

	/*
	 * 		For every link to that output, (inputLinks) calculate its weight correction term
	 * 		and update the link with it.
	*/
	for(unsigned int iInputLink = 0; iInputLink < (this->GetInputLink()).size(); iInputLink++){
		NeuralLink<T> * pInputLink = (this->GetInputLink()).at(iInputLink);
		double Zj = pInputLink->GetLastTranslatedSignal();
		double dWeightCorrectionTerm = Zj*dErrorInformationTerm;
		//std::cout << "dWeightCorrectionTerm: " << dWeightCorrectionTerm << std::endl;
		pInputLink->SetWeightCorrectionTerm(LearningRate*dWeightCorrectionTerm);


		/*
		 * 		Then output unit has to tell the hidden neurons the value of it ErrorInformationTerm, so we are setting its value
		 * 		in the link object.
		 */

		pInputLink->SetErrorInFormationTerm(dErrorInformationTerm);
	}


	return res;
}

template <typename T>
void Neuron<T>::PerformWeightsUpdating()
{

}

template <typename T>
void HiddenLayerNeuronDecorator<T>::PerformWeightsUpdating()
{
	for(unsigned int iInputLink = 0; iInputLink < (this->GetInputLink()).size(); iInputLink++){
		NeuralLink<T> * pInputLink = (this->GetInputLink()).at(iInputLink);

		pInputLink->UpdateWeight();
		//std::cout<<"";
	}
}

template <typename T>
void OutputLayerNeuronDecorator<T>::PerformWeightsUpdating()
{
	for(unsigned int iInputLink = 0; iInputLink < (this->GetInputLink()).size(); iInputLink++){
		NeuralLink<T> * pInputLink = (this->GetInputLink()).at(iInputLink);

		pInputLink->UpdateWeight();
		//std::cout<<"";
	}
}

template <typename T>
void Neuron<T>::ShowNeuronState()
{
	/*
	 * 		Printing out Neuron's link's weights
	*/

	for(unsigned int iNumOfOutLinks = 0; iNumOfOutLinks < mLinksToNeurons.size(); iNumOfOutLinks++ ){
		NeuralLink<T> * pNeuralLink = mLinksToNeurons.at(iNumOfOutLinks);
		std::cout << "    Link index: " << iNumOfOutLinks << std::endl;
		std::cout << "      Weight: " << pNeuralLink->GetWeight() << "; Weight correction term: " << pNeuralLink->GetWeightCorrectionTerm();
		std::cout << std::endl;

	}
}

template class Neuron<double>;
template class Neuron<float>;
template class Neuron<int>;

template class OutputLayerNeuronDecorator<double>;
template class OutputLayerNeuronDecorator<float>;
template class OutputLayerNeuronDecorator<int>;

template class HiddenLayerNeuronDecorator<double>;
template class HiddenLayerNeuronDecorator<float>;
template class HiddenLayerNeuronDecorator<int>;
