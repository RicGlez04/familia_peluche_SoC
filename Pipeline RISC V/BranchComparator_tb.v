module BranchComparator_tb();

reg Branch, Zero, Jump;
wire PCSrc;

BranchComparator DUT (
    .Branch(Branch),
    .Zero(Zero),
    .Jump(Jump),
    .PCSrc(PCSrc)
);

initial
	begin
		// Todo en 0, no hay salto (PCSrc = 0)
		Branch = 0; Zero = 0; Jump = 0;
		#10;

		// Branch pero la condicion NO se cumple (Zero = 0), no salta (PCSrc = 0)
		Branch = 1; Zero = 0; Jump = 0;
		#10;

		//Branch y la condicion SI se cumple (Zero = 1), salta (PCSrc = 1)
		Branch = 1; Zero = 1; Jump = 0;
		#10;

		// No es Branch, pero es un salto incondicional (PCSrc = 1)
		Branch = 0; Zero = 0; Jump = 1;
		#10;

		// Ambos activos (Branch y Jump), sigue saltando (PCSrc = 1)
		Branch = 1; Zero = 1; Jump = 1;
		#10;
	end

endmodule