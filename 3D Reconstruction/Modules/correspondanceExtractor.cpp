#include "correspondanceExtractor.h"

using namespace std;
using namespace cv;

CorrespondanceExtractor::CorrespondanceExtractor()
{
	
}

void CorrespondanceExtractor::init(std::string dataSet)
{
	if (dataSet == "dinosaur")
	{
		//if (!loadMatches(dataSet + ".alx"))
		//{
			readImages("data/dinosaur/im (", 37, ").ppm");
			findMatches();
			saveMatches(dataSet + ".alx");
		//}
		double Kdata[9] = {	3217.328669180762, -78.606641008226180, 289.8672403229193,
							0,					2292.424143977958,  -1070.516234777778,
							0,					0,					1};
		K = cv::Mat(3,3,CV_64FC1,Kdata).clone();
	}

}

void CorrespondanceExtractor::readImages(string prefix, int numberOfImages, string postfix)
{
	//string prefix = directory + "/im (";
	//string postfix = ").ppm";
	string fileName;
	int step = 1;
	int fileNo = 1;
	

	int numberToRead = numberOfImages;
	//numberToRead = 5;

	//for(int counter = 1; counter <= numberToRead ; counter++)

	for(int counter = 1; counter <= numberOfImages ; counter++)
	{    
		if (fileNo > numberOfImages)
		{
			fileNo = fileNo - numberOfImages;
		}

		fileName = prefix + to_string(fileNo) + postfix;

		cout << "Filename: " << fileName << endl;
		imageList.push_back(imread(fileName));
		if(imageList.back().empty())
			cout << "\t Could not read file!\n";

		fileNo = fileNo + step;
    }

	/*
	string fileName, fileNumber;
	string fileBeginning = "data/dinosaur/viff.";
	string fileEnding = ".ppm";

	int endingNumber = 36;

    for(int fileNo = 0; fileNo <= endingNumber ;fileNo++)
	{    
		fileNumber =  to_string(fileNo);

		int zeros = 3 - fileNumber.size();

		for (int i = 0; i < zeros; i++)
		{
			fileNumber = "0" + fileNumber;
		}

		fileName = fileBeginning + fileNumber + fileEnding;

		cout << "Filename: " << fileName << endl;
		imageList.push_back(imread(fileName));
		if(imageList.back().empty())
			cout << "\t Could not read file!\n";
    }
	*/
}

void CorrespondanceExtractor::findMatches()
{
	// FEATURES
	/*
	// ---------- HARRIS detector -----------
	int		maxNumberOfFeatures =	200;		// Maximum number of features to return
	double	qualityLevel =			0.01;		// Remove features with worse than 99 % of the best eigenvalue,			0.01
	double	minDistance =			15;			// Minimal Euclidiean distance between features,						3
	int		blockSize =				7;			// Size of averaging mask when calculating neighbourhood covariance,	3
	bool	useHarris =				true;		// Use Harris or die,													false
	double	k =						0.04;		// Parameter for Harris detector: Har(x,y) = det - k * trace^2,			0.04
	GoodFeaturesToTrackDetector featureDetector(maxNumberOfFeatures, qualityLevel, minDistance, blockSize, useHarris, k);
	*/

	// ---------- HARRIS detector -----------
	int		maxNumberOfFeatures =	200;		// Maximum number of features to return
	double	qualityLevel =			0.01;		// Remove features with worse than 99 % of the best eigenvalue,			0.01
	double	minDistance =			15;			// Minimal Euclidiean distance between features,						3
	int		blockSize =				3;			// Size of averaging mask when calculating neighbourhood covariance,	3
	bool	useHarris =				false;		// Use Harris or die,													false
	double	k =						0.04;		// Parameter for Harris detector: Har(x,y) = det - k * trace^2,			0.04
	GoodFeaturesToTrackDetector featureDetector(maxNumberOfFeatures, qualityLevel, minDistance, blockSize, useHarris, k);


	/*
	// ---------- SIFT detector -----------
	double siftThreshold =		0.04;
	double siftEdgeThreshold =	10;

	SiftFeatureDetector featureDetector(siftThreshold, siftEdgeThreshold);
	*/

	// DESCRIPTORS

	/*
	// ---------- BRIEF Descriptor -----------
	int descriptorSize = 64;	// Size of descriptor = 16, 32 or 64
	BriefDescriptorExtractor descriptorExtractor(descriptorSize);
	*/
	
	// ---------- SIFT Descriptor -----------
	double magnification =		3;			// 3
	bool isNormalize =			true;		// true
	bool recalculateAngles =	true;		// true 
	SiftDescriptorExtractor descriptorExtractor(magnification, isNormalize, recalculateAngles);

	// MATCHER
	BFMatcher matcher(NORM_L2, true);

	Mat image1;
	Mat image2;

	
	vector<KeyPoint> keypoints1;
	vector<KeyPoint> keypoints2;
	Mat featureImage1;
	Mat featureImage2;
	Mat descriptors1;
	Mat descriptors2;
	int maxGeometricDistance = 200;

	vector<vector<pointPair>> sequenceMatches;

	for (vector<Mat>::iterator i = imageList.begin(); i < imageList.end() - 1; i++)
	{
		featureDetector.detect(*i, keypoints1);
		featureDetector.detect(*(i+1), keypoints2);

		drawKeypoints(*i, keypoints1, featureImage1);
		drawKeypoints(*(i+1), keypoints2, featureImage2);

		descriptorExtractor.compute(*i, keypoints1, descriptors1);
		descriptorExtractor.compute(*(i+1), keypoints2, descriptors2);

		// Match descriptors between images
		vector<DMatch> rawMatches, matches;
		matcher.match(descriptors1, descriptors2, rawMatches);
		
		// Remove bad matches
		for (vector<DMatch>::iterator j = rawMatches.begin(); j < rawMatches.end(); j++)
		{
			if ((*j).distance < maxGeometricDistance)
			{
				matches.push_back(*j);
			}
		}

		vector<pointPair> pointPairs;

		for (int k=0; k<matches.size(); k++)
		{

			pointPair temp;
			temp.p1.x = keypoints1[matches[k].queryIdx].pt.x;
			temp.p1.y = keypoints1[matches[k].queryIdx].pt.y;
			temp.p2.x = keypoints2[matches[k].trainIdx].pt.x;
			temp.p2.y = keypoints2[matches[k].trainIdx].pt.y;

			//cout << "p1: " << temp.p1 << endl;
			//cout << "p2: " << temp.p2 << endl;

			pointPairs.push_back(temp);
		}

		matchesVector.push_back(pointPairs);

		//matchesVector->push_back(matches);
		cout << "matches: " << matches.size() << endl;
		
		Mat matchImage;
		drawMatches(*i, keypoints1, *(i+1), keypoints2, matches, matchImage);

		imshow("win1", matchImage);
		waitKey(1);
	}
}

void CorrespondanceExtractor::saveMatches(string filename)
{
	ofstream os(filename, ios::binary);
	if(!os)
	{
		cout << "Error saving to file " << filename << endl;
	}

	int imageCounter = 0;

	os << "# Correspondances for each imagepair in sequnce \n";

	for (vector<vector<pointPair>>::iterator i = matchesVector.begin(); i < matchesVector.end(); i++)
	{
		imageCounter++;
		os << "# Imagepair " << imageCounter << "\n";
		for (vector<pointPair>::iterator j = i->begin(); j < i->end(); j++)
		{
			os << "p ";
			os << j->p1.x << " ";
			os << j->p1.y << " ";
			os << j->p2.x << " ";
			os << j->p2.y << " ";
			os << "\n";
		}
		os << "# Partly Done \n\n";
	}

	cout << "Successfully saved" << endl;
}

bool CorrespondanceExtractor::loadMatches(string filename)
{
	ifstream is(filename, ios::binary);
	if(!is)
	{
		cout << "Error loading from file " << filename << endl;
		cout << "Recalculating correspondances" << endl;
		return false;
	}
	
	string line;
	istringstream ss;
	vector<pointPair> pointPairs;

	while (getline(is, line))
	{
		
		if(line.substr(0,2) == "p ")
		{
			// A pointpair

			ss = istringstream(line.substr(2));
			pointPair temp;
			ss >> temp.p1.x;
			ss >> temp.p1.y;
			ss >> temp.p2.x;
			ss >> temp.p2.y;

			pointPairs.push_back(temp);
		}
		else if(line.substr(0,13) == "# Partly Done")
		{
			// Done loadng one imagepair
			matchesVector.push_back(pointPairs);
			pointPairs.clear();
		}
		else if(line.substr(0,1) == "#")
		{
			// A comment line, ignore.
		}
		else
		{
			// Not suported line
		}
	}

	cout << "Successfully loaded" << endl;
	return true;
}

void CorrespondanceExtractor::getBAPoints(int imagePair, vector<Point2d>& BAPoints1, vector<Point2d>& BAPoints2)
{
	for (vector<pointPair>::iterator i = matchesVector[imagePair].begin(); i < matchesVector[imagePair].end(); i++)
	{
		BAPoints1.push_back(i->p1);
		BAPoints2.push_back(i->p2);
	}
}



bool ChainGreaterThan(const std::vector<ChainNode> & left, const std::vector<ChainNode> & right) { return left.size() > right.size(); }

struct Point2dLessThan {
	bool operator() (const cv::Point2d & left, const cv::Point2d & right) { return left.x < left.y || (left.x == right.x && left.y < right.y); }
};

void CorrespondanceExtractor::generateChains()
{
	std::vector<std::map<cv::Point2d,int,Point2dLessThan>> lookUp; 
	lookUp.resize(matchesVector.size(),std::map<cv::Point2d,int,Point2dLessThan>());	// Pre-allocate
	std::map<cv::Point2d,int,Point2dLessThan>::iterator finder;

	// Add the points for the first image
	for(int c = 0; c < matchesVector[0].size(); c++)
	{
		chain.push_back(std::vector<ChainNode>());
		chain.back().push_back(ChainNode(matchesVector[0][c].p1));
		lookUp.back()[matchesVector[0][c].p2] = c;
		startOnImage.push_back(0);
	}

	int chainNumber = 0;
	for(int i = 1; i < matchesVector.size()-1; i++)	//For each two images with correspondances
	{
		for(int c = 0; c < matchesVector[i].size(); c++)
		{
			finder = lookUp[i-1].find(matchesVector[i][c].p1);
			if(finder != lookUp[i-1].end())
			{
				chainNumber = finder->second;
				chain[chainNumber].push_back(ChainNode(matchesVector[i][c].p1));
			}
			else  // Possible improvement, remove chains that wasn't extended last frame and which have length 1 or 2 or something!
			{
				chainNumber = chain.size()-1;
				chain.push_back(std::vector<ChainNode>());
				chain.back().push_back(ChainNode(matchesVector[i][c].p1));
				startOnImage.push_back(i);
			}
			lookUp.back()[matchesVector[i][c].p2] = chainNumber;
		}
	}

	// Add the points for the last image
	int i = matchesVector.size()-2;
	for(int c = 0; c < matchesVector[i].size(); c++)
	{
		finder = lookUp[i-1].find(matchesVector[i][c].p1);
		if(finder != lookUp[i-1].end())
		{
			chainNumber = finder->second;
			chain[chainNumber].push_back(ChainNode(matchesVector[i][c].p2));
		}
		else
		{
			// Big error
			std::cout << "!!!!!!!!!!!!!!!! W T F (chain generator...) !!!!!!!!!!!!!!!!!! \n";
		}
	}

	// Sort on how long the chains are (might be faster if using lists, but then the map must contain list iterators to chain (or similar) for fast access)
	std::sort(chain.begin(), chain.end(), ChainGreaterThan);
}
