import { IsNotEmpty, IsString, Length } from 'class-validator';

export class CreateConductorDto {
  @IsString()
  @Length(11, 11)
  @IsNotEmpty()
  cpf: string;

  @IsString()
  @IsNotEmpty()
  name: string;
}
