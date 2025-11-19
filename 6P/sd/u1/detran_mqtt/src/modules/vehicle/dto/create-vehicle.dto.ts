import { IsNotEmpty, IsNumber, IsString, Length } from 'class-validator';

export class CreateVehicleDto {
  @IsString()
  @Length(7, 7)
  @IsNotEmpty()
  sign: string;

  @IsString()
  @IsNotEmpty()
  model: string;

  @IsNumber()
  @IsNotEmpty()
  value: number;

  @IsString()
  @Length(11, 11)
  @IsNotEmpty()
  conductorCpf: string;
}
