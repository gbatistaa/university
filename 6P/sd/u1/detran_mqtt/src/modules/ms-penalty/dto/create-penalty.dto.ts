import { IsNotEmpty, IsNumber, IsString } from 'class-validator';

export class CreatePenaltyDto {
  @IsString()
  @IsNotEmpty()
  description: string;

  @IsNumber()
  @IsNotEmpty()
  pontuation: number;

  @IsString()
  @IsNotEmpty()
  vehicleSign: string;
}
