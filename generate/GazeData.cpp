#include "GazeData.h"
#include "Render\Matrix.h"
#include "Render\Transformation.h"
#include <random>
#include <iostream>
#include <Windows.h>
#include <Pathcch.h>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>

#undef LoadImage

const std::regex CGazeData::s_regData( R"a((?:"((?:[^"]|"")*)"\s+|(\S+)\s+)\d+.*)a" );
const std::regex CGazeData::s_regLabel( R"a(([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+))).*)a" );

bool CGazeData::Export( std::vector<CData> &vecData, const std::wstring &sPath, unsigned uValBatchSize, double dTrainValRatio )
{
	{
		double dBatchCount = vecData.size( ) * ( 1 - dTrainValRatio ) / uValBatchSize;
		dBatchCount = round( dBatchCount );
		dTrainValRatio = 1 - dBatchCount * uValBatchSize / vecData.size( );
	}

	std::random_device rng;
	std::mt19937 urng( rng( ) );
	std::shuffle( vecData.begin( ), vecData.end( ), urng );
	const std::vector<CData> vecTrain( vecData.begin( ), vecData.begin( ) + (int) ( vecData.size( ) * dTrainValRatio ) );
	const std::vector<CData> vecVal( vecData.begin( ) + (int) ( vecData.size( ) * dTrainValRatio ), vecData.end( ) );

	std::wcout << "Exporting to \"" << sPath << "\"" << std::endl;
	std::wcout << "Training data  : " << vecTrain.size( ) << std::endl;
	std::wcout << "Validation data: " << vecVal.size( ) << std::endl;

	std::wstring sTrain;
	std::wstring sTrainData;
	std::wstring sTrainLabel;
	std::wstring sVal;
	std::wstring sValData;
	std::wstring sValLabel;
	WCHAR szFullPattern[ MAX_PATH ];
	PathCchCombine( szFullPattern, MAX_PATH, sPath.c_str( ), L"train" );
	sTrain = std::wstring( szFullPattern );
	PathCchCombine( szFullPattern, MAX_PATH, sTrain.c_str( ), L"data.txt" );
	sTrainData = std::wstring( szFullPattern );
	PathCchCombine( szFullPattern, MAX_PATH, sTrain.c_str( ), L"label.csv" );
	sTrainLabel = std::wstring( szFullPattern );
	
	PathCchCombine( szFullPattern, MAX_PATH, sPath.c_str( ), L"val" );
	sVal = std::wstring( szFullPattern );
	PathCchCombine( szFullPattern, MAX_PATH, sVal.c_str( ), L"data.txt" );
	sValData = std::wstring( szFullPattern );
	PathCchCombine( szFullPattern, MAX_PATH, sVal.c_str( ), L"label.csv" );
	sValLabel = std::wstring( szFullPattern );

	//Create export directory
	if( !CreateDirectory( sPath.c_str( ), nullptr ) && GetLastError( ) != ERROR_ALREADY_EXISTS )
	{
		std::wcerr << "Unable to create directory \"" << sPath << "\"" << std::endl;
		return false;
	}
	

	//Create train directory
	while( true )
		if( !CreateDirectory( sTrain.c_str( ), nullptr ) )
		{
			if( GetLastError( ) != ERROR_ALREADY_EXISTS )
			{
				std::wcerr << "Unable to create directory \"" << sTrain << "\"" << std::endl;
				return false;
			}
			
			//Directory exists, try to remove it
			if( !RemoveDirectory( sTrain.c_str( ) ) )
			{
				std::wcerr << "Unable to clear directory \"" << sTrain << "\"" << std::endl;
				break;	//Unable to remove directory, continue with full one
			}
			
			//Directory removed, next iteration will create an empty one
		}
		else
			break;	//Empty directory created

	//Create val directory
	while( true )
		if( !CreateDirectory( sVal.c_str( ), nullptr ) )
		{
			if( GetLastError( ) != ERROR_ALREADY_EXISTS )
			{
				std::wcerr << "Unable to create directory \"" << sVal << "\"" << std::endl;
				return false;
			}
			
			//Directory exists, try to remove it
			if( !RemoveDirectory( sVal.c_str( ) ) )
			{
				std::wcerr << "Unable to clear directory \"" << sVal << "\"" << std::endl;
				break;	//Unable to remove directory, continue with full one
			}
			
			//Directory removed, next iteration will create an empty one
		}
		else
			break;	//Empty directory created

	//Export train data
	std::fstream smData( sTrainData, std::fstream::out );
	if( !smData.is_open( ) )
	{
		std::wcerr << "Unable to open file \"" << sTrainData << "\" for writing" << std::endl;
		return false;
	}

	std::fstream smLabel( sTrainLabel, std::fstream::out );
	if( !smLabel.is_open( ) )
	{
		std::wcerr << "Unable to open file \"" << sTrainLabel << "\" for writing" << std::endl;
		smData.close( );
		return false;
	}

	std::uniform_real_distribution<double> disScale( 0.9, 1.1 );
	std::uniform_real_distribution<double> disOffset( -0.05, 0.05 );

	unsigned uCurrent = 0;
	unsigned uTotal = (unsigned) vecTrain.size( );
	std::wcout << "Exporting training data" << std::endl;
	wprintf( L"%3.0f%% (%u / %u)", (double) uCurrent / uTotal * 100, uCurrent, uTotal );
	fflush( stdout );
	uCurrent++;
	for( CGazeData data : vecTrain )
	{
		data.LoadImage( );
		data.ScaleFace( CVector<2>( { disScale( urng ), disScale( urng ) } ), CVector<2>( { disOffset( urng ), disOffset( urng ) } ) );
		PathCchCombine( szFullPattern, MAX_PATH, sTrain.c_str( ), data.sImage.c_str( ) );
		if( !cv::imwrite( std::string( szFullPattern, szFullPattern + wcslen( szFullPattern ) ), data.matImage( data.rectFace ) ) )
		{
			std::wcerr << "Failed to write image to \"" << szFullPattern << "\"" << std::endl;
			continue;
		}
		smData << std::string( data.sImage.begin( ), data.sImage.end( ) ) << " 1" << std::endl;
		smLabel << data.ToString( ) << std::endl;
		wprintf( L"\r%3.0f%% (%u / %u)", (double) uCurrent / uTotal * 100, uCurrent, uTotal );
		fflush( stdout );
		uCurrent++;
	}
	std::wcout << std::endl;

	smData.close( );
	smLabel.close( );

	//Export validataion data
	smData.open( sValData, std::fstream::out );
	if( !smData.is_open( ) )
	{
		std::wcerr << "Unable to open file \"" << sValData << "\" for writing" << std::endl;
		return false;
	}

	smLabel.open( sValLabel, std::fstream::out );
	if( !smLabel.is_open( ) )
	{
		std::wcerr << "Unable to open file \"" << sValLabel << "\" for writing" << std::endl;
		smData.close( );
		return false;
	}

	uCurrent = 0;
	uTotal = (unsigned) vecVal.size( );
	std::wcout << "Exporting validation data" << std::endl;
	wprintf( L"%3.0f%% (%u / %u)", (double) uCurrent / uTotal * 100, uCurrent, uTotal );
	fflush( stdout );
	uCurrent++;
	for( CGazeData data : vecVal )
	{
		data.LoadImage( );
		data.ScaleFace( CVector<2>( { disScale( urng ), disScale( urng ) } ), CVector<2>( { disOffset( urng ), disOffset( urng ) } ) );
		PathCchCombine( szFullPattern, MAX_PATH, sVal.c_str( ), data.sImage.c_str( ) );
		if( !cv::imwrite( std::string( szFullPattern, szFullPattern + wcslen( szFullPattern ) ), data.matImage( data.rectFace ) ) )
		{
			std::wcerr << "Failed to write image to \"" << szFullPattern << "\"" << std::endl;
			continue;
		}
		smData << std::string( data.sImage.begin( ), data.sImage.end( ) ) << " 1" << std::endl;
		smLabel << data.ToString( ) << std::endl;
		wprintf( L"\r%3.0f%% (%u / %u)", (double) uCurrent / uTotal * 100, uCurrent, uTotal );
		fflush( stdout );
		uCurrent++;
	}
	std::wcout << std::endl;

	smData.close( );
	smLabel.close( );

	return false;
}

std::vector<CGazeData> CGazeData::LoadData( const std::wstring &sPath )
{
	std::vector<CGazeData> vecData;

	std::wstring sData;
	std::wstring sLabel;
	{
		WCHAR szFullPattern[ MAX_PATH ];
		PathCchCombine( szFullPattern, MAX_PATH, sPath.c_str( ), L"data.txt" );
		sData = std::wstring( szFullPattern );
		PathCchCombine( szFullPattern, MAX_PATH, sPath.c_str( ), L"label.csv" );
		sLabel = std::wstring( szFullPattern );
	}

	std::fstream smData( sData, std::fstream::in );
	if( !smData.is_open( ) )
	{
		std::wcerr << "Unable to open data file \"" << sData << "\" for reading" << std::endl;
		return vecData;
	}

	std::fstream smLabel( sLabel, std::fstream::in );
	if( !smData.is_open( ) )
	{
		std::wcerr << "Unable to open label file \"" << sData << "\" for reading" << std::endl;
		smData.close( );
		return vecData;
	}

	std::string sDataLine;
	std::string sLabelLine;
	while( std::getline( smData, sDataLine ) && std::getline( smLabel, sLabelLine ) )
		vecData.emplace_back( sDataLine, sLabelLine, sPath );

	smLabel.close( );
	smData.close( );
	return vecData;
}

cv::Rect CGazeData::FindTemplate( const cv::Mat &matImage, const cv::Mat &matTemplate )
{
	//Scale imput images
	double dScale = 720.0 / matImage.cols;
	cv::Mat matImageS;
	cv::Mat matTemplateS;
	cv::resize( matImage, matImageS, cv::Size( 720, (int) ( matImage.rows * dScale ) ) );
	cv::resize( matTemplate, matTemplateS, cv::Size( (int) ( matTemplate.cols * dScale ), (int) ( matTemplate.rows * dScale ) ) );
	
	//Create the result matrix
	cv::Mat matResult( matImageS.cols - matTemplateS.cols + 1, matImageS.rows - matTemplateS.rows + 1, CV_32FC1 );

	//Do the Matching and Normalize
	cv::matchTemplate( matImageS, matTemplateS, matResult, CV_TM_SQDIFF );
	cv::normalize( matResult, matResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat( ) );

	//Localizing the best match with minMaxLoc
	double dMinVal;
	double dMaxVal;
	cv::Point ptMinLoc;
	cv::Point ptMaxLoc;
	cv::minMaxLoc( matResult, &dMinVal, &dMaxVal, &ptMinLoc, &ptMaxLoc, cv::Mat( ) );
	
	ptMinLoc = cv::Point( (int) ( ptMinLoc.x / dScale ), (int) ( ptMinLoc.y / dScale ) );
	return cv::Rect( ptMinLoc, cv::Point( (int) ( ptMinLoc.x + matTemplateS.cols / dScale ), (int) ( ptMinLoc.y + matTemplateS.rows / dScale ) ) );
}

CGazeData::CGazeData( const CData &data ) :
	CData( data ),
	rayEyeLeft( data.vec3EyeLeft, data.vec3GazePoint - data.vec3EyeLeft ),
	rayEyeRight( data.vec3EyeRight, data.vec3GazePoint - data.vec3EyeRight )
{
	
}

CGazeData::CGazeData( const std::string &sData, const std::string &sLabel, const std::wstring &sPath )
{
	std::smatch match;
	std::regex_match( sData, match, s_regData );
	if( !match.size( ) )
		throw 0;

	if( match[ 1 ].matched )
		sImage = StrToWStr( std::regex_replace( match[ 1 ].str( ), std::regex( "\"\"" ), "\"" ) );
	else
		sImage = StrToWStr( match[ 2 ].str( ) );

	sRootPath = sPath;

	std::regex_match( sLabel, match, s_regLabel );
	if( !match.size( ) )
		throw 1;

	rayEyeLeft = CRay( CVector<3>( { 0.5, 0, 0 } ), CVector<2>( { std::stod( match[ 1 ].str( ) ), std::stod( match[ 2 ].str( ) ) } ) );
	rayEyeRight = CRay( CVector<3>( { -0.5, 0, 0 } ), CVector<2>( { std::stod( match[ 3 ].str( ) ), std::stod( match[ 4 ].str( ) ) } ) );
	vec3EyeLeft = CVector<3>( { std::stod( match[ 5 ].str( ) ), std::stod( match[ 6 ].str( ) ) } );
	vec3EyeRight = CVector<3>( { std::stod( match[ 7 ].str( ) ), std::stod( match[ 8 ].str( ) ) } );
}

CData CGazeData::MergeReference( const std::vector<CData> &vecData )
{
	//Find reference in vector
	std::vector<CData>::const_iterator it = std::find_if( vecData.begin( ), vecData.end( ), [ & ]( const CData &data )
	{
		return this->sImage == data.sImage;
	} );

	if( it == vecData.end( ) )
	{
		std::wcout << "Warning: Imported image \"" << sImage << "\" not found in dataset" << std::endl;
		return CData( );
	}

	CData data( *it );
	if( !data.LoadImage( ) || !LoadImage( ) )
		return CData( );

	rectFace = FindTemplate( data.matImage, matImage );
	ptEyeLeft = cv::Point( (int) ( vec3EyeLeft[ 0 ] * matImage.cols ), (int) ( vec3EyeLeft[ 1 ] * matImage.rows ) );
	ptEyeRight = cv::Point( (int) ( vec3EyeRight[ 0 ] * matImage.cols ), (int) ( vec3EyeRight[ 1 ] * matImage.rows ) );
	
	//Copy image and eye coordinates
	matImage = data.matImage.clone( );
	vec3EyeLeft = data.vec3EyeLeft;
	vec3EyeRight = data.vec3EyeRight;

	//Rays are in local space of the face, transform to global space
	CTransformation matTransform( GetFaceTransformation( ) );
	rayEyeLeft = matTransform * rayEyeLeft;
	rayEyeRight = matTransform * rayEyeRight;
	
	//Move rays in position
	CVector<3> vec3FacePos = ( vec3EyeLeft + vec3EyeRight ) / 2.0;
	rayEyeLeft += vec3FacePos;
	rayEyeRight += vec3FacePos;

	//Calculate gaze point as the point of shortest distance between the eye rays
	CVector<2> vec2Gaze = rayEyeLeft.PointOfShortestDistance( rayEyeRight );
	vec3GazePoint = ( rayEyeLeft( vec2Gaze[ 0 ] ) + rayEyeRight( vec2Gaze[ 1 ] ) ) / 2.0;
	return data;
}
	
std::string CGazeData::ToString( unsigned uPrecision ) const
{
	std::ostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	std::wcout.setf( std::ios_base::fixed, std::ios_base::floatfield );
	std::wcout.precision( 6 );

	CTransformation matTransform( GetFaceTransformation( ).Invert( ) );
	CVector<2> vec2EyeLeft( ( matTransform * rayEyeLeft ).AmplitudeRepresentation( ) );
	CVector<2> vec2EyeRight( ( matTransform * rayEyeRight ).AmplitudeRepresentation( ) );

#if 1
	CRay rayRef( matTransform * rayEyeLeft );
	rayRef.m_vec3Dir.Normalize( );
	CVector<2> vec2( rayRef.AmplitudeRepresentation( ) );
	std::wcout << "Amp: " << vec2 << std::endl;
	CRay rayTest( CVector<3>( { 0.5, 0, 0 } ), vec2 );
	std::wcout << "Ref: " << rayRef << std::endl;
	std::wcout << "Test: " << rayTest << std::endl;
#else
	CRay rayRef( CVector<3>( { 0.5, 0, 0 } ), CVector<3>( { 1, 1, 1 } ) );
	rayRef.m_vec3Dir.Normalize( );
	CVector<2> vec2( rayRef.AmplitudeRepresentation( ) );
	std::wcout << "Amp: " << vec2 << std::endl;
	CRay rayTest( CVector<3>( { 0.5, 0, 0 } ), CVector<2>( { 0.5, 0 } ) );
	std::wcout << "Ref: " << rayRef << std::endl;
	std::wcout << "Test: " << rayTest << std::endl;
#endif

	out << vec2EyeLeft[ 0 ] << "," << vec2EyeLeft[ 1 ] << "," << vec2EyeRight[ 0 ] << "," << vec2EyeRight[ 1 ] << ",";
	out << (double) ptEyeLeft.x / rectFace.width << "," << (double) ptEyeLeft.y / rectFace.height << ",";
	out << (double) ptEyeRight.x / rectFace.width << "," << (double) ptEyeRight.y / rectFace.height;
	return out.str( );
}