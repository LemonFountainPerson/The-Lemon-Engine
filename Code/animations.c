#include "animations.h"




int LoopParticleAnimation(Object *particle)
{
	if (particle->arg2 < 1 || particle->arg2 > 999)
	{
		particle->arg2 = 3;
	}

	if (particle->arg1 < 1 || particle->animationTick % particle->arg2 != 0)
	{
		return EXECUTION_UNNECESSARY;
	
	}

	int firstSprite = 1;
	int lastSprite = 1;
	int animateType = 1;

	// Add a new case whenever you make a new particle here
	switch(particle->currentAnimation)
	{
		case SPARKLE:
		lastSprite = 7;
		break;

		default:
		break;
	}

	if (lastSprite < firstSprite)
	{
		return INVALID_DATA;
	}

	if (particle->arg2 < 1 || particle->arg2 > 999)
	{
		particle->arg2 = 3;
	}


	// Add a new case for specific animation sequences
	switch (animateType)
	{
		case 2:
		{
			int onOddLoop = 1 - ( particle->animationTick / ((lastSprite - firstSprite + 1) * particle->arg2) ) % 2;

			if (onOddLoop == 1)
			{
				particle->currentSprite++;
			}
			else
			{
				particle->currentSprite--;
			}

			if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
			{
				particle->arg1--;

				if (onOddLoop == 1)
				{
					particle->currentSprite = lastSprite;
				}
				else
				{
					particle->currentSprite = firstSprite;
				}
			}

		} break;


		case -2:
		{
			int onOddLoop = 1 - ( particle->animationTick / ((lastSprite - firstSprite + 1) * particle->arg2) ) % 2;

			if (onOddLoop == 1)
			{
				particle->currentSprite--;
			}
			else
			{
				particle->currentSprite++;
			}


			if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
			{
				particle->arg1--;

				if (onOddLoop == 1)
				{
					particle->currentSprite = firstSprite;
				}
				else
				{
					particle->currentSprite = lastSprite;
				}
			}

		} break;


		case -1:
		{
			particle->currentSprite--;

			if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
			{
				particle->arg1--;

				particle->currentSprite = lastSprite;
			}

		} break;

		default:
		{
			particle->currentSprite++;

			if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
			{
				particle->arg1--;

				particle->currentSprite = firstSprite;
			}

		} break;
	}

	return 0;
}



int genericAnimationHandler(Object *inputObject)
{
	inputObject->animationTick++;

	int firstSprite = 1;
	int lastSprite = 1;
	int ticksBeforeNextFrame = 4;
	int animateType = 1;


	switch (animateType)
	{
		case -1:
		{
			inputObject->currentSprite = lastSprite - (inputObject->animationTick / ticksBeforeNextFrame);

			if (inputObject->currentSprite < firstSprite || inputObject->currentSprite > lastSprite)
			{
				inputObject->currentSprite = lastSprite;
				inputObject->animationTick = 0;
			}
		} break;

		default:
		{
			inputObject->currentSprite = firstSprite + (inputObject->animationTick / ticksBeforeNextFrame);

			if (inputObject->currentSprite < firstSprite || inputObject->currentSprite > lastSprite)
			{
				inputObject->currentSprite = firstSprite;
				inputObject->animationTick = 0;
			}
		} break;
	}



}