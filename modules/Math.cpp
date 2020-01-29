// --------------------------------------------------------------------------------
//
//                      EpicBasic Math Library
//
// --------------------------------------------------------------------------------

const auto& _ebP_Cos = cos ;
const auto& _ebP_Sin = sin ;
const auto& _ebP_Tan = tan ;

const auto& _ebP_ACos = acos ;
const auto& _ebP_ASin = asin ;
const auto& _ebP_ATan2 = atan2 ;

const auto& _ebP_ATanH = atanh ;

const auto& _ebP_Exp = exp ;
const auto& _ebP_Log = log ;
const auto& _ebP_Log10 = log10 ;
const auto& _ebP_Pow = pow ;
const auto& _ebP_Sqr = sqrt ;

const auto& _ebP_RoundUp = ceil ;
const auto& _ebP_RoundDown = floor ;

int _ebP_RandomRange (int maximum, int minimum)
{
	int length = (maximum - minimum)+1; // +1 for inclusive
	// TODO: error if length is negative or 0. Error if minimum < 0.
	
	int res = rand(); // [0 to RAND_MAX)
	res = res % length; // [0 to length]
	
	return (minimum + res); // [min to max]
}

int _ebP_Random (int maximum)
{
	return _ebP_RandomRange (maximum, 0);
}

void _ebP_RandomSeed (int seed)
{
	srand (seed);
}

void _ebP_RandomData (char* pointer, int byteLength)
{
	// for now, do each byte individually
	for (auto x=0; x < byteLength; ++x)
	{
		char c = static_cast<char> (_ebP_Random(255)); // [0, 255]
		
		*(pointer+x) = c;
	}
}

// seed the generator at program start
static int INTERNAL_Math_programStart = []() -> int
{
	// use the current time, should be good enough for now
	srand(time(NULL));
	
	return 1; // success
}();
